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

#ifndef SVGCACHE_H_INCLUDED
#define SVGCACHE_H_INCLUDED

class QSvgRenderer;

namespace SvgCache
{
  void init();
  QSvgRenderer* get( const char* resource_path );
  void clear();
};


#endif /* SVGCACHE_H_INCLUDED */
