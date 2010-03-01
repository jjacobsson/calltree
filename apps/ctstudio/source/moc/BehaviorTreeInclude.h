/*******************************************************************************
 * Copyright (c) 2009-04-25 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#ifndef BEHAVOIRTREEINCLUDE_H_INCLUDED
#define BEHAVOIRTREEINCLUDE_H_INCLUDED

#include "BehaviorTreeSceneItem.h"
#include "../GraphicsItemTypes.h"

class BehaviorTreeInclude: public BehaviorTreeSceneItem
{
  Q_OBJECT
public:

  BehaviorTreeInclude( Include* );

  enum
  {
    Type = BehaviorTreeIncludeType
  };

  bool isType( int type ) const {
    if( Type == type )
      return true;
    return BehaviorTreeSceneItem::isType( type );
  }

  void destroyResources( BehaviorTreeContext ctx );

protected:

  Include* m_Include;

};

#endif /*BEHAVOIRTREEINCLUDE_H_INCLUDED*/
