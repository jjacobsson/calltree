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

#ifndef ICONCACHE_H_INCLUDED
#define ICONCACHE_H_INCLUDED

struct NamedSymbol;
class QIcon;

namespace IconCache
{
  void init();
  QIcon* get( NamedSymbol* );
  QIcon* get( const char* resource_path );
  void clear();
};


#endif /* ICONCACHE_H_INCLUDED */
