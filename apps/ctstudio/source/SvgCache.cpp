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

#include "SvgCache.h"

#include <other/lookup3.h>

#include <QtSvg/QSvgRenderer>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <algorithm>

#include <string.h>
#include <malloc.h>

#include <btree/btree_data.h>
#include <btree/btree_func.h>

#include "standard_resources.h"

namespace SvgCache
{

struct SvgCacheEntry
{
  hash_t m_Hash;
  QSvgRenderer* m_Renderer;
};

class CacheTableLookupPred
{
public:
  bool operator()( const SvgCacheEntry& l, const SvgCacheEntry& r ) const
  {
    return l.m_Hash < r.m_Hash;
  }
} g_Pred;

struct SvgCacheData
{
  SvgCacheEntry* m_Entries;
  int m_Size;
  int m_Capacity;
} g_SvgCache;

const int g_GrowCount = 128;

void grow()
{
  int alloc = sizeof(SvgCacheEntry) * g_SvgCache.m_Capacity
      + sizeof(SvgCacheEntry) * g_GrowCount;
  SvgCacheEntry* n = (SvgCacheEntry*)malloc( alloc );
  if( g_SvgCache.m_Entries )
    memcpy( n, g_SvgCache.m_Entries, sizeof(SvgCacheEntry) * g_SvgCache.m_Capacity );

  SvgCacheEntry* o = g_SvgCache.m_Entries;
  g_SvgCache.m_Entries = n;
  g_SvgCache.m_Capacity += g_GrowCount;

  free(o);
}

void insert( const SvgCacheEntry& l )
{
  if( g_SvgCache.m_Size == g_SvgCache.m_Capacity )
    grow();

  SvgCacheEntry* it = std::lower_bound( g_SvgCache.m_Entries,
    g_SvgCache.m_Entries + g_SvgCache.m_Size, l, g_Pred );

  if( it != g_SvgCache.m_Entries + g_SvgCache.m_Size )
  {
    int count = (g_SvgCache.m_Size * sizeof(SvgCacheEntry)) - ((int)it
        - (int)g_SvgCache.m_Entries);
    memmove( it + 1, it, count );
  }

  it->m_Hash     = l.m_Hash;
  it->m_Renderer = l.m_Renderer;
  g_SvgCache.m_Size++;
}

void init()
{
  g_SvgCache.m_Entries  = 0x0;
  g_SvgCache.m_Size     = 0;
  g_SvgCache.m_Capacity = 0;
}

QSvgRenderer* get( Node* n )
{
  if( !n )
    return get( g_NodeSVGResourcePaths[E_GRIST_UNKOWN] );

  Parameter* opts = 0x0;
  Locator l;
  int gt = n->m_Grist.m_Type;

  switch( gt )
  {
  case E_GRIST_UNKOWN:
    break;
  case E_GRIST_SEQUENCE:
    break;
  case E_GRIST_SELECTOR:
    break;
  case E_GRIST_PARALLEL:
    break;
  case E_GRIST_DYN_SELECTOR:
    break;
  case E_GRIST_SUCCEED:
    break;
  case E_GRIST_FAIL:
    break;
  case E_GRIST_WORK:
    break;
  case E_GRIST_TREE:
    break;
  case E_GRIST_ACTION:
    opts = n->m_Grist.m_Action.m_Action->m_Options;
    l = n->m_Grist.m_Action.m_Action->m_Locator;
    break;
  case E_GRIST_DECORATOR:
    opts = n->m_Grist.m_Decorator.m_Decorator->m_Options;
    l = n->m_Grist.m_Decorator.m_Decorator->m_Locator;
    break;
  case E_MAX_GRIST_TYPES:
    break;
  }

  if( opts )
    opts = find_by_hash( opts, hashlittle( "cts_icon" ) );
  if( opts && !safe_to_convert( *opts, E_VART_STRING ) )
    opts = 0x0;

  QSvgRenderer* svgr = 0x0;

  if( opts )
  {
    QFileInfo fi( l.m_Buffer );
    QDir parent_dir( fi.absoluteDir() );
    QString qstr = parent_dir.absoluteFilePath( QString( as_string( *opts )->m_Raw ) );
    std::string stdstr( qstr.toStdString() );
    svgr = get( stdstr.c_str() );
  }
  if( !svgr )
    svgr = get( g_NodeSVGResourcePaths[gt] );

  return svgr;
}

QSvgRenderer* get( const char* resource_path )
{
  if( !resource_path )
    return 0x0;

  SvgCacheEntry l;
  l.m_Hash     = hashlittle( resource_path );
  l.m_Renderer = 0x0;

  SvgCacheEntry* it = std::lower_bound( g_SvgCache.m_Entries,
    g_SvgCache.m_Entries + g_SvgCache.m_Size, l, g_Pred );

  if( it == g_SvgCache.m_Entries + g_SvgCache.m_Size || it->m_Hash != l.m_Hash )
  {
    //make a new renderer
    l.m_Renderer = new QSvgRenderer( QString( resource_path ) );
    if( !l.m_Renderer->isValid() )
    {
      delete l.m_Renderer;
      l.m_Renderer = 0x0;
    }
    else
      insert( l );
  }
  else
  {
    l = *it;
  }

  return l.m_Renderer;
}

void clear()
{
  for( int i = 0; i < g_SvgCache.m_Size; ++i )
    delete g_SvgCache.m_Entries[i].m_Renderer;
  free( g_SvgCache.m_Entries );
  init();
}

}
