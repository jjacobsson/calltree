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

#include "BehaviorTreeScene.h"
#include "BehaviorTreeNode.h"
#include "BehaviorTreeTree.h"
#include "ParamConnectorPlug.h"
#include "../NodeToNodeArrow.h"
#include "../standard_resources.h"

#include <other/lookup3.h>
#include <btree/btree.h>

#include <QtSvg/QGraphicsSvgItem>

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleValidator>
#include <QtGui/QIntValidator>

#include <string.h>

BehaviorTreeNode::BehaviorTreeNode( Node* n, BehaviorTreeSceneItem* parent ) :
  BehaviorTreeSceneItem( parent ), m_Node( n ), m_DraggingArrow( 0x0 ),
      m_Label( 0x0 )
{
  m_Graphics = new QGraphicsSvgItem( g_NodeSVGResourcePaths[n->m_Grist.m_Type],
    this );
  setupLabel();
  setupTooltip();
}

void BehaviorTreeNode::setupPropertyEditor()
{
  QLabel* label = 0x0;

  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*> ( scene() );
  if( !s )
    return;

  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_UNKOWN:
    label = new QLabel( tr(
      "I have no idea what kind of node you have selected mate..." ) );
    break;
  case E_GRIST_SEQUENCE:
    label = new QLabel( tr( "Sequence" ) );
    break;
  case E_GRIST_SELECTOR:
    label = new QLabel( tr( "Selector" ) );
    break;
  case E_GRIST_PARALLEL:
    label = new QLabel( tr( "Parallel" ) );
    break;
  case E_GRIST_DYN_SELECTOR:
    label = new QLabel( tr( "Dynamic Selector" ) );
    break;
  case E_GRIST_SUCCEED:
    label = new QLabel( tr( "Success node" ) );
    break;
  case E_GRIST_FAIL:
    label = new QLabel( tr( "Fail node" ) );
    break;
  case E_GRIST_WORK:
    label = new QLabel( tr( "Working node" ) );
    break;
  case E_GRIST_DECORATOR:
    {
      NamedSymbol* ns = find_symbol( s->getFullContext(),
        m_Node->m_Grist.m_Decorator.m_Decorator->m_Id.m_Hash );
      if( ns && ns->m_Type == E_ST_DECORATOR )
      {
        setupPropertyEditorForParamaters(
          m_Node->m_Grist.m_Decorator.m_Parameters,
          ns->m_Symbol.m_Decorator->m_Declarations );
      }
    }
    break;
  case E_GRIST_ACTION:
    {
      NamedSymbol* ns = find_symbol( s->getFullContext(),
        m_Node->m_Grist.m_Action.m_Action->m_Id.m_Hash );
      if( ns && ns->m_Type == E_ST_ACTION )
      {
        setupPropertyEditorForParamaters(
          m_Node->m_Grist.m_Action.m_Parameters,
          ns->m_Symbol.m_Action->m_Declarations );
      }
    }
    break;
  case E_MAX_GRIST_TYPES:
    /* Warning killer */
    break;
  }

  if( label )
  {
    m_PropertyWidget = new QWidget;
    QBoxLayout* layout = new QBoxLayout( QBoxLayout::TopToBottom,
      m_PropertyWidget );
    layout->addWidget( label, 0, Qt::AlignHCenter | Qt::AlignVCenter );
  }
}

QRectF BehaviorTreeNode::boundingRect() const
{
  QRectF rect = m_Graphics->boundingRect();
  if( m_Label )
    rect |= m_Label->boundingRect().translated( m_Label->pos() );
  return rect;
}

QRectF BehaviorTreeNode::layoutBoundingRect() const
{
  return m_Graphics->boundingRect().translated( m_Graphics->pos() );
}

qreal BehaviorTreeNode::layoutOffset() const
{
  return m_Graphics->pos().rx();
}

void BehaviorTreeNode::destroyResources( BehaviorTreeContext ctx )
{
  unlink_from_parent_and_siblings( m_Node );
  Parameter* v = 0x0;
  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    v = m_Node->m_Grist.m_Decorator.m_Parameters;
    break;
  case E_GRIST_ACTION:
    v = m_Node->m_Grist.m_Action.m_Parameters;
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
    Parameter* n = v->m_Next;
    free_object( ctx, v );
    v = n;
  }
  free_object( ctx, m_Node );
  m_Node = 0x0;
}

BehaviorTreeSceneItem* BehaviorTreeNode::firstChild()
{
  Node* n = get_first_child( m_Node );
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

void BehaviorTreeNode::paramChanged( QObject* editor, hash_t hash )
{
  Parameter* p = find_by_hash( get_parameters( m_Node ), hash );
  BehaviorTreeScene* s = qobject_cast<BehaviorTreeScene*> ( scene() );
  if( !p || !s )
    return;

  switch( p->m_Type )
  {
  case E_VART_BOOL:
    {
      QCheckBox* cb = qobject_cast<QCheckBox*> ( editor );
      bool checked = cb->checkState() == Qt::Checked;
      if( checked != p->m_Data.m_Bool )
      {
        p->m_Data.m_Bool = checked;
        signalModified(false);
      }
    }
    break;
  case E_VART_FLOAT:
    {
      QLineEdit* e = qobject_cast<QLineEdit*> ( editor );
      bool ok;
      float f = e->text().toFloat( &ok );
      if( ok && f != p->m_Data.m_Float )
      {
        p->m_Data.m_Float = f;
        signalModified(false);
      }
    }
    break;
  case E_VART_INTEGER:
    {
      QLineEdit* e = qobject_cast<QLineEdit*> ( editor );
      bool ok;
      int i = e->text().toInt( &ok );
      if( ok && i != p->m_Data.m_Integer )
      {
        p->m_Data.m_Integer = i;
        signalModified(false);
      }
    }
    break;
  case E_VART_STRING:
    {
      QLineEdit* e = qobject_cast<QLineEdit*> ( editor );
      QByteArray ba( e->text().toAscii() );
      hash_t hash_old = hashlittle( p->m_Data.m_String.m_Raw );
      hash_t hash_new = hashlittle( ba.constData() );
      if( hash_old != hash_new )
      {
        p->m_Data.m_String.m_Raw = register_string( s->getTreeContext(),
          ba.constData() );
        signalModified(false);
      }
    }
    break;
  case E_VART_UNDEFINED:
  case E_MAX_VARIABLE_TYPE:
    // Warning Killers
    break;
  }
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
  font.setPixelSize( 64 );
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

void BehaviorTreeNode::setupPropertyEditorForParamaters( Parameter* set,
  Parameter* dec )
{
  if( dec == 0x0 )
    return;

  m_PropertyWidget = new QWidget;
  QFormLayout* form = new QFormLayout( m_PropertyWidget );

  Parameter* it = dec;
  while( it )
  {
    Parameter* v = find_by_hash( set, it->m_Id.m_Hash );
    switch( it->m_Type )
    {
    case E_VART_BOOL:
      {
        QCheckBox* cb = new QCheckBox( it->m_Id.m_Text );
        if( v )
          cb->setChecked( v->m_Data.m_Bool );
        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, cb );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( cb, SIGNAL( stateChanged( int ) ), conn,
          SLOT( checkBoxChanged( int ) ) );
        form->addRow( cb );
      }
      break;
    case E_VART_FLOAT:
      {
        QLabel* l = new QLabel( it->m_Id.m_Text );
        QLineEdit* e = new QLineEdit;
        if( v )
          e->setText( QString( "%1" ).arg( v->m_Data.m_Float ) );
        e->setValidator( new QDoubleValidator( 0x0 ) );
        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, e );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( e, SIGNAL( textChanged( const QString& ) ), conn,
          SLOT( lineEditChanged( const QString& ) ) );
        form->addRow( l, e );
      }
      break;
    case E_VART_INTEGER:
      {
        QLabel* l = new QLabel( it->m_Id.m_Text );
        QLineEdit* e = new QLineEdit;
        if( v )
          e->setText( QString( "%1" ).arg( v->m_Data.m_Integer ) );
        e->setValidator( new QIntValidator( 0x0 ) );
        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, e );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( e, SIGNAL( textChanged( const QString& ) ), conn,
          SLOT( lineEditChanged( const QString& ) ) );
        form->addRow( l, e );
      }
      break;
    case E_VART_STRING:
      {
        QLabel* l = new QLabel( it->m_Id.m_Text );
        QLineEdit* e = new QLineEdit;
        if( v )
          e->setText( v->m_Data.m_String.m_Raw );
        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, e );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( e, SIGNAL( textChanged( const QString& ) ), conn,
          SLOT( lineEditChanged( const QString& ) ) );
        form->addRow( l, e );
      }
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      // Warning Killers
      break;
    }
    it = it->m_Next;
  }
}

void BehaviorTreeNode::setupRelinkage()
{
  m_BeforeRelinkage.m_Pare = m_Node->m_Pare;
  m_BeforeRelinkage.m_Prev = m_Node->m_Prev;
  m_BeforeRelinkage.m_Next = m_Node->m_Next;

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

  unlink_from_parent_and_siblings( m_Node );
}

void BehaviorTreeNode::executeRelinkage()
{
  m_Node->m_Pare = m_Relinkage.m_Parent;

  if( m_Relinkage.m_Sibling )
  {
    if( m_Relinkage.m_BeforeSibling )
    {
      if( m_Relinkage.m_Sibling->m_Prev )
        m_Relinkage.m_Sibling->m_Prev->m_Next = m_Node;
      else
        set_first_child( m_Node->m_Pare, m_Node );

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
    set_first_child( m_Node->m_Pare, m_Node );
  }

  if( m_Node->m_Pare.m_Node != m_BeforeRelinkage.m_Pare.m_Node ||
      m_Node->m_Pare.m_Tree != m_BeforeRelinkage.m_Pare.m_Tree ||
      m_Node->m_Pare.m_Type != m_BeforeRelinkage.m_Pare.m_Type ||
      m_Node->m_Prev != m_BeforeRelinkage.m_Prev ||
      m_Node->m_Next != m_BeforeRelinkage.m_Next )
    signalModified(false);

}

void BehaviorTreeNode::lookForRelinkTarget()
{
  QList<QGraphicsItem*> coll( m_Graphics->collidingItems() );
  QList<QGraphicsItem*>::iterator it, it_e( coll.end() );
  for( it = coll.begin(); it != it_e; ++it )
  {
    BehaviorTreeSceneItem* btsi = qgraphicsitem_cast<BehaviorTreeSceneItem*> (
      *it );
    if( !btsi || btsi == this )
      continue;

    if( btsi->isType( BehaviorTreeNode::Type ) )
    {
      BehaviorTreeNode* item = (BehaviorTreeNode*)btsi;
      Node* p = item->m_Node;

      if( !accepts_more_children( p ) )
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

  m_Relinkage.m_Sibling = get_first_child( m_Relinkage.m_Parent );

  BehaviorTreeNode* item;
  Node* n = m_Relinkage.m_Sibling;

  if( !n )
    return;

  item = (BehaviorTreeNode*)n->m_UserData;
  qreal x = scenePos().x();
  qreal tx = item->scenePos().x();
  qreal best = qAbs( x - tx );

  m_Relinkage.m_Sibling = n;
  if( tx < x )
    m_Relinkage.m_BeforeSibling = false;
  else
    m_Relinkage.m_BeforeSibling = true;

  while( n )
  {
    item = (BehaviorTreeNode*)n->m_UserData;
    tx = item->scenePos().x();
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

  QString s =
      QString( "Drag X: %1 Best X: %2 Before: %3" ).arg( x ).arg( best ).arg(
        m_Relinkage.m_BeforeSibling );

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

