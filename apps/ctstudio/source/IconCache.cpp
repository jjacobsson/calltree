/*******************************************************************************
 * Copyright (c) 2010-03-30 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "IconCache.h"
#include "SvgCache.h"

#include <other/lookup3.h>

#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtSvg/QSvgRenderer>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <algorithm>

#include <string.h>
#include <malloc.h>

#include <btree/btree_data.h>
#include <btree/btree_func.h>

#include "standard_resources.h"

namespace IconCache
{

struct IconCacheEntry
{
  hash_t m_Hash;
  QIcon* m_Icon;
};

class CacheTableLookupPred
{
public:
  bool operator()( const IconCacheEntry& l, const IconCacheEntry& r ) const
  {
    return l.m_Hash < r.m_Hash;
  }
} g_Pred;

struct IconCacheData
{
  IconCacheEntry* m_Entries;
  int m_Size;
  int m_Capacity;
} g_IconCache;

const int g_GrowCount = 128;

void grow()
{
  int alloc = sizeof(IconCacheEntry) * g_IconCache.m_Capacity
      + sizeof(IconCacheEntry) * g_GrowCount;
  IconCacheEntry* n = (IconCacheEntry*)malloc( alloc );
  if( g_IconCache.m_Entries )
    memcpy( n, g_IconCache.m_Entries, sizeof(IconCacheEntry) * g_IconCache.m_Capacity );

  IconCacheEntry* o = g_IconCache.m_Entries;
  g_IconCache.m_Entries = n;
  g_IconCache.m_Capacity += g_GrowCount;

  free(o);
}

void insert( const IconCacheEntry& l )
{
  if( g_IconCache.m_Size == g_IconCache.m_Capacity )
    grow();

  IconCacheEntry* it = std::lower_bound( g_IconCache.m_Entries,
    g_IconCache.m_Entries + g_IconCache.m_Size, l, g_Pred );

  if( it != g_IconCache.m_Entries + g_IconCache.m_Size )
  {
    int count = (g_IconCache.m_Size * sizeof(IconCacheEntry)) - ((int)it
        - (int)g_IconCache.m_Entries);
    memmove( it + 1, it, count );
  }

  it->m_Hash = l.m_Hash;
  it->m_Icon = l.m_Icon;
  g_IconCache.m_Size++;
}

void init()
{
  g_IconCache.m_Entries  = 0x0;
  g_IconCache.m_Size     = 0;
  g_IconCache.m_Capacity = 0;
}

QIcon* get( NamedSymbol* ns )
{
  Parameter* opts = 0x0;
  Locator l;
  int gt = E_GRIST_UNKOWN;
  switch( ns->m_Type )
  {
  case E_ST_UNKOWN:
    /* Warning Killer */
    break;
  case E_ST_TREE:
    gt = E_GRIST_TREE;
    break;
  case E_ST_ACTION:
    opts = ns->m_Symbol.m_Action->m_Options;
    l = ns->m_Symbol.m_Action->m_Locator;
    gt = E_GRIST_ACTION;
    break;
  case E_ST_DECORATOR:
    opts = ns->m_Symbol.m_Decorator->m_Options;
    l = ns->m_Symbol.m_Decorator->m_Locator;
    gt = E_GRIST_DECORATOR;
    break;
  case E_MAX_SYMBOL_TYPES:
    /* Warning Killer */
    break;
  }

  if( opts )
    opts = find_by_hash( opts, hashlittle( "cts_icon" ) );
  if( opts && !safe_to_convert( *opts, E_VART_STRING ) )
    opts = 0x0;

  QIcon* icon = 0x0;

  if( opts )
  {
    QFileInfo fi( l.m_Buffer );
    QDir parent_dir( fi.absoluteDir() );
    QString qstr = parent_dir.absoluteFilePath( QString( as_string( *opts )->m_Raw ) );
    std::string stdstr( qstr.toStdString() );
    icon = get( stdstr.c_str() );
  }

  if( !icon )
    icon = get( g_NodeSVGResourcePaths[gt] );

  return icon;
}

QIcon* get( const char* resource_path )
{
  if( !resource_path )
    return 0x0;

  IconCacheEntry l;
  l.m_Hash = hashlittle( resource_path );
  l.m_Icon = 0x0;

  IconCacheEntry* it = std::lower_bound( g_IconCache.m_Entries,
    g_IconCache.m_Entries + g_IconCache.m_Size, l, g_Pred );

  if( it == g_IconCache.m_Entries + g_IconCache.m_Size || it->m_Hash != l.m_Hash )
  {
    QSvgRenderer* svgr = SvgCache::get( resource_path );
    if( svgr )
    {
      QImage img( 128, 128, QImage::Format_ARGB32_Premultiplied );
      img.fill( 0 );
      QPainter p( &img );
      svgr->render( &p );
      p.end();
      if( !img.isNull() )
      {
        l.m_Icon = new QIcon( QPixmap::fromImage( img ) );
        insert( l );
      }
    }
  }
  else
  {
    l = *it;
  }

  return l.m_Icon;
}

void clear()
{
  for( int i = 0; i < g_IconCache.m_Size; ++i )
    delete g_IconCache.m_Entries[i].m_Icon;
  free( g_IconCache.m_Entries );
  init();
}

}
