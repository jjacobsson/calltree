/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "BehaviorTreeNode.h"
#include "BehaviorTreeTree.h"
#include "../NodeToNodeArrow.h"
#include "../standard_resources.h"
#include <btree/btree.h>

#include <QtGui/QtGui>
#include <QtSvg/QGraphicsSvgItem>

#include <string.h>

BehaviorTreeNode::BehaviorTreeNode( Node* n, BehaviorTreeSceneItem* parent )
	: BehaviorTreeSceneItem( parent )
	, m_Node( n )
	, m_DraggingArrow( 0x0 )
	, m_Label( 0x0 )
{
  m_Graphics = new QGraphicsSvgItem( g_NodeSVGResourcePaths[n->m_Grist.m_Type], this );
  setupLabel();
  setupTooltip();
}

QRectF BehaviorTreeNode::boundingRect() const
{
  QRectF rect = m_Graphics->boundingRect();
  if( m_Label )
    rect |= m_Label->boundingRect().translated(m_Label->pos());
  return rect;
}

void BehaviorTreeNode::destroyResources( BehaviorTreeContext ctx )
{
  UnlinkNodeFromParentAndSiblings( m_Node );
  Variable* v = 0x0;
  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    v = m_Node->m_Grist.m_Decorator.m_Arguments;
    break;
  case E_GRIST_ACTION:
    v = m_Node->m_Grist.m_Action.m_Arguments;
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_MAX_GRIST_TYPES:
    /* Warning killers */
    v = 0x0;
    break;
  }
  while( v )
  {
    Variable* n = v->m_Next;
    BehaviorTreeContextFreeObject( ctx, v );
    v = n;
  }
  BehaviorTreeContextFreeObject( ctx, m_Node );
  m_Node = 0x0;
}

BehaviorTreeSceneItem* BehaviorTreeNode::firstChild()
{
  Node* n = GetFirstChild( m_Node );
  if( n )
    return (BehaviorTreeSceneItem*)n->m_UserData;
  return 0x0;
}

BehaviorTreeSceneItem* BehaviorTreeNode::nextSibling()
{
  if( m_Node->m_Next )
    return (BehaviorTreeSceneItem*)m_Node->m_Next->m_UserData;
  return 0x0;
}

bool BehaviorTreeNode::validForDrop() const
{
  return m_Relinkage.m_Parent.m_Type != E_NP_UNKOWN;
}

void BehaviorTreeNode::dragMove()
{
  lookForRelinkTarget();
  BehaviorTreeSceneItem::dragMove();
}

void BehaviorTreeNode::dragBegin()
{
	if( parentItem() )
	{
		BehaviorTreeNode* p = (BehaviorTreeNode*)parentItem();
		m_DraggingArrow = findArrowTo( p );
		p->removeArrow( m_DraggingArrow );
		m_DraggingArrow->setDashed( true );
		m_DraggingArrow->setStartAndEnd( this, p );

		QPointF position( scenePos() );
		setParentItem( 0x0 );
		setPos( position );
	}
	else
	{
		m_DraggingArrow = new NodeToNodeArrow( this, 0x0, scene() );
		m_DraggingArrow->setDashed( true );
		addArrow( m_DraggingArrow );
	}

	setupRelinkage();

	BehaviorTreeSceneItem::dragBegin();
}

void BehaviorTreeNode::dragEnd()
{
  switch( m_Relinkage.m_Parent.m_Type )
  {
  case E_NP_NODE:
    dragEndNodeParent();
    break;
  case E_NP_TREE:
    dragEndTreeParent();
    break;
  default:
    removeArrow( m_DraggingArrow );
    delete m_DraggingArrow;
    m_DraggingArrow = 0x0;
    break;
  }
  executeRelinkage();
  BehaviorTreeSceneItem::dragEnd();
}

void BehaviorTreeNode::setupLabel()
{
  const char* str = 0x0;
  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    str = m_Node->m_Grist.m_Decorator.m_Decorator->m_Id.m_Text;
    break;
  case E_GRIST_ACTION:
    str = m_Node->m_Grist.m_Action.m_Action->m_Id.m_Text;
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_MAX_GRIST_TYPES:
    /* Warning killers */
    break;
  }

  if( !str )
    return;

  QFont font;
  font.setPixelSize(64);
  m_Label = new QGraphicsTextItem( str, this );
  m_Label->setFont( font );
  QPointF p;
  QRectF r( m_Label->boundingRect() );

  p.rx() = 0.0;

  float move_node = (r.width() - 256.0) / 2.0;
  if( move_node > 0 )
    m_Graphics->setPos( move_node, 0 );
  else
    p.rx() = -move_node;

  p.ry() = 256.0;
  m_Label->setPos( p );
}

void BehaviorTreeNode::setupTooltip()
{
  QString str;
  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_UNKOWN:
    str += tr( "Unknown" );
    break;
  case E_GRIST_SEQUENCE:
    str += tr( "Sequence" );
    break;
  case E_GRIST_SELECTOR:
    str += tr( "Selector" );
    break;
  case E_GRIST_PARALLEL:
    str += tr( "Parallel" );
    break;
  case E_GRIST_DYN_SELECTOR:
    str += tr( "Dynamic Selector" );
    break;
  case E_GRIST_SUCCEED:
    str += tr( "Succeed" );
    break;
  case E_GRIST_FAIL:
    str += tr( "Fail" );
    break;
  case E_GRIST_WORK:
    str += tr( "Work" );
    break;
  case E_GRIST_DECORATOR:
    str += tr( "Decorator, " );
    str += m_Node->m_Grist.m_Decorator.m_Decorator->m_Id.m_Text;
    break;
  case E_GRIST_ACTION:
    str += tr( "Action, " );
    str += m_Node->m_Grist.m_Action.m_Action->m_Id.m_Text;
    break;
  case E_MAX_GRIST_TYPES:
    /* Warning killer */
    break;
  }
  setToolTip( str );
}

void BehaviorTreeNode::setupRelinkage()
{
  m_Relinkage.m_Parent = m_Node->m_Pare;

  if( m_Node->m_Prev )
  {
    m_Relinkage.m_Sibling = m_Node->m_Prev;
    m_Relinkage.m_BeforeSibling = false;
  }
  else if( m_Node->m_Next )
  {
    m_Relinkage.m_Sibling = m_Node->m_Next;
    m_Relinkage.m_BeforeSibling = true;
  }
  else
  {
    m_Relinkage.m_Sibling = 0x0;
    m_Relinkage.m_BeforeSibling = false;
  }

  memcpy( &m_Previous, &m_Relinkage, sizeof(Relinkage) );

  UnlinkNodeFromParentAndSiblings( m_Node );
}

void BehaviorTreeNode::executeRelinkage()
{

  if( memcmp( &m_Relinkage, &m_Previous, sizeof(Relinkage) ) != 0 )
    signalModified();

  m_Node->m_Pare = m_Relinkage.m_Parent;

  if( m_Relinkage.m_Sibling )
  {
    if( m_Relinkage.m_BeforeSibling )
    {
      if( m_Relinkage.m_Sibling->m_Prev )
        m_Relinkage.m_Sibling->m_Prev->m_Next = m_Node;
      else
        SetFirstChild( m_Node->m_Pare, m_Node );

      m_Node->m_Prev = m_Relinkage.m_Sibling->m_Prev;

      m_Relinkage.m_Sibling->m_Prev = m_Node;
      m_Node->m_Next = m_Relinkage.m_Sibling;
    }
    else
    {
      if( m_Relinkage.m_Sibling->m_Next )
        m_Relinkage.m_Sibling->m_Next->m_Prev = m_Node;
      m_Node->m_Next = m_Relinkage.m_Sibling->m_Next;
      m_Relinkage.m_Sibling->m_Next = m_Node;
      m_Node->m_Prev = m_Relinkage.m_Sibling;
    }
  }
  else
  {
    m_Node->m_Next = 0x0;
    m_Node->m_Prev = 0x0;
    SetFirstChild( m_Node->m_Pare, m_Node );
  }
}

void BehaviorTreeNode::lookForRelinkTarget()
{
  QList<QGraphicsItem*> coll( m_Graphics->collidingItems() );
  QList<QGraphicsItem*>::iterator it, it_e( coll.end() );
  for( it = coll.begin(); it != it_e; ++it )
  {
    BehaviorTreeSceneItem* btsi = qgraphicsitem_cast<BehaviorTreeSceneItem*>( *it );
    if( !btsi || btsi == this )
      continue;

    if( btsi->isType( BehaviorTreeNode::Type ) )
    {
      BehaviorTreeNode* item = (BehaviorTreeNode*)btsi;
      Node* p = item->m_Node;

      if( !AcceptsMoreChildren( p ) )
        continue;

      // Current parent does not need evaluation.
      if( p == m_Relinkage.m_Parent.m_Node )
        continue;

      Relinkage t;

      t.m_Parent.m_Type = E_NP_NODE;
      t.m_Parent.m_Node = p;
      t.m_BeforeSibling = true;

      if( m_DraggingArrow )
      {
        m_DraggingArrow->setStartAndEnd( this, item );
        m_DraggingArrow->setVisible( true );
      }
      m_Relinkage = t;
    }
    else if( btsi->isType( BehaviorTreeTree::Type ) )
    {
      BehaviorTreeTree* item = (BehaviorTreeTree*)btsi;
      BehaviorTree* p = item->GetTree();

      if( p == m_Relinkage.m_Parent.m_Tree )
        continue;

      Relinkage t;
      t.m_Parent.m_Type = E_NP_TREE;
      t.m_Parent.m_Tree = p;
      t.m_BeforeSibling = true;

      if( m_DraggingArrow )
      {
        m_DraggingArrow->setStartAndEnd( this, item );
        m_DraggingArrow->setVisible( true );
      }
      m_Relinkage = t;

    }
  }

  m_Relinkage.m_Sibling = GetFirstChild( m_Relinkage.m_Parent );

  BehaviorTreeNode* item;
  Node* n = m_Relinkage.m_Sibling;

  if( !n )
    return;

  item        = (BehaviorTreeNode*)n->m_UserData;
  qreal x     = scenePos().x();
  qreal tx    = item->scenePos().x();
  qreal best  = qAbs( x - tx );

  m_Relinkage.m_Sibling = n;
  if( tx < x )
      m_Relinkage.m_BeforeSibling = false;
  else
      m_Relinkage.m_BeforeSibling = true;

  while( n )
  {
      item    = (BehaviorTreeNode*)n->m_UserData;
      tx      = item->scenePos().x();
      qreal d = qAbs( x - tx );
      if( d < best )
      {
          m_Relinkage.m_Sibling = n;
          if( tx < x )
              m_Relinkage.m_BeforeSibling = false;
          else
              m_Relinkage.m_BeforeSibling = true;
          best = d;
      }
      n = n->m_Next;
  }

  QString s = QString( "Drag X: %1 Best X: %2 Before: %3").arg(x).arg(best).arg(m_Relinkage.m_BeforeSibling);

  emit relinkTargetMessage( s, 2000 );
}

void BehaviorTreeNode::dragEndNodeParent()
{
  BehaviorTreeNode* p =
      (BehaviorTreeNode*)m_Relinkage.m_Parent.m_Node->m_UserData;
  setPos( p->mapFromScene( scenePos() ) );
  setParentItem( p );

  p->addArrow( m_DraggingArrow );
  m_DraggingArrow->setStartAndEnd( p, this );
  m_DraggingArrow->setDashed( false );
  m_DraggingArrow = 0x0;
}

void BehaviorTreeNode::dragEndTreeParent()
{
  BehaviorTreeTree* p =
      (BehaviorTreeTree*)m_Relinkage.m_Parent.m_Tree->m_UserData;
  setPos( p->mapFromScene( scenePos() ) );
  setParentItem( p );

  p->addArrow( m_DraggingArrow );
  m_DraggingArrow->setStartAndEnd( p, this );
  m_DraggingArrow->setDashed( false );
  m_DraggingArrow = 0x0;
}

