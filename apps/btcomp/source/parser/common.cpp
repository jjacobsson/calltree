/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include "common.h"

ParserContext::ParserContext()
  : yyscanner( 0x0 )
  , m_NewLines( 0 )
  , m_Tree( 0x0 )
  , m_File( 0x0 )
  , m_PPCurrScope( 0 )
  , m_PPUntilScope( 0 )
{
}


