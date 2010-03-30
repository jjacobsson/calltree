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
#include "../SvgCache.h"

#include <other/lookup3.h>
#include <btree/btree.h>

#include <QtSvg/QGraphicsSvgItem>

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleValidator>
#include <QtGui/QIntValidator>

#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>

#include <string.h>

BehaviorTreeNode::BehaviorTreeNode( BehaviorTreeContext ctx, Node* n,
  BehaviorTreeSceneItem* parent ) :
  BehaviorTreeSceneItem( ctx, parent ), m_Node( n ), m_DraggingArrow( 0x0 ),
      m_Label( 0x0 )
{
  m_Graphics = new QGraphicsSvgItem( this );
  m_Graphics->setSharedRenderer( SvgCache::get( n ) );
  setupLabel();
  setupTooltip();

  if( m_Node->m_Grist.m_Type == E_GRIST_TREE &&
      m_Node->m_Grist.m_Tree.m_Tree &&
      m_Node->m_Grist.m_Tree.m_Tree->m_UserData )
  {
    BehaviorTreeSceneItem* btsi = (BehaviorTreeSceneItem*)(m_Node->m_Grist.m_Tree.m_Tree->m_UserData );
    connect( btsi, SIGNAL(itemDeleted()), this, SLOT(deleteThis()) );
  }
}

BehaviorTreeNode::~BehaviorTreeNode()
{
  unlink_from_parent_and_siblings( m_Node );
  free_node( m_Context, m_Node );
  m_Node = 0x0;
}

void BehaviorTreeNode::setupPropertyEditor()
{
  QLabel* label = 0x0;

  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_GRIST_TREE:
    label = new QLabel( g_NodeNames[m_Node->m_Grist.m_Type] );
    break;
  case E_GRIST_DECORATOR:
    setupPropertyEditorForParamaters(
      m_Node->m_Grist.m_Decorator.m_Parameters,
      m_Node->m_Grist.m_Decorator.m_Decorator->m_Declarations );
    break;
  case E_GRIST_ACTION:
    setupPropertyEditorForParamaters(
      m_Node->m_Grist.m_Action.m_Parameters,
      m_Node->m_Grist.m_Action.m_Action->m_Declarations );
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

BehaviorTreeSceneItem* BehaviorTreeNode::getParent()
{
  BehaviorTreeSceneItem* r = 0x0;
  switch( m_Node->m_Pare.m_Type )
  {
  case E_NP_TREE:
    r = (BehaviorTreeSceneItem*)m_Node->m_Pare.m_Tree->m_UserData;
    break;
  case E_NP_NODE:
    r = (BehaviorTreeSceneItem*)m_Node->m_Pare.m_Node->m_UserData;
    break;
  case E_NP_UNKOWN:
  case E_MAX_NODE_PARENT_TYPES:
    break;
  }
  return r;
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
  if( m_Relinkage.m_Parent.m_Type == E_NP_UNKOWN )
    return false;
  return true;
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

void BehaviorTreeNode::dragFail()
{
  removeArrow( m_DraggingArrow );
  delete m_DraggingArrow;
  m_DraggingArrow = 0x0;
  BehaviorTreeSceneItem::dragFail();
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
        signalModified( false );
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
        signalModified( false );
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
        signalModified( false );
      }
    }
    break;
  case E_VART_STRING:
    {
      QLineEdit* e = qobject_cast<QLineEdit*> ( editor );
      QByteArray ba( e->text().toAscii() );
      hash_t hash_new, hash_old;

      if( e->text().isEmpty() )
        hash_new = hashlittle( "" );
      else
        hash_new = hashlittle( ba.constData() );

      if( p->m_Data.m_String.m_Raw )
        hash_old = hashlittle( p->m_Data.m_String.m_Raw );
      else
        hash_old = hashlittle( "" );

      if( hash_old != hash_new )
      {
        p->m_Data.m_String.m_Raw = register_string( s->getTreeContext(),
          ba.constData() );
        p->m_Data.m_String.m_Parsed = p->m_Data.m_String.m_Raw;
        signalModified( false );
      }
    }
    break;
  case E_VART_HASH:
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
  case E_GRIST_TREE:
    str = m_Node->m_Grist.m_Tree.m_Tree->m_Id.m_Text;
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
  QString str( g_NodeNames[m_Node->m_Grist.m_Type] );
  switch( m_Node->m_Grist.m_Type )
  {
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
    break;
  case E_GRIST_DECORATOR:
    str += ", ";
    str += m_Node->m_Grist.m_Decorator.m_Decorator->m_Id.m_Text;
    break;
  case E_GRIST_ACTION:
    str += ", ";
    str += m_Node->m_Grist.m_Action.m_Action->m_Id.m_Text;
    break;
  case E_GRIST_TREE:
    str += ", ";
    str += m_Node->m_Grist.m_Tree.m_Tree->m_Id.m_Text;
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
  QTableWidget* tw = new QTableWidget;
  m_PropertyWidget = tw;

  tw->setColumnCount( 3 );

  {
    QStringList headers;
    headers.push_back( tr( "Param" ) );
    headers.push_back( tr( "Type" ) );
    headers.push_back( tr( "Value" ) );
    tw->setHorizontalHeaderLabels( headers );
    tw->verticalHeader()->hide();
  }

  Parameter* it = dec;
  while( it )
  {
    Parameter* v = find_by_hash( set, it->m_Id.m_Hash );

    QLabel* l = new QLabel( it->m_Id.m_Text );
    QLabel* tl = 0x0;
    QWidget* e = 0x0;
    switch( it->m_Type )
    {
    case E_VART_BOOL:
      {
        tl = new QLabel( tr( "bool" ) );
        QWidget* cb_cont = new QWidget;
        QCheckBox* cb = new QCheckBox( cb_cont );
        cb->move( 5, 3 );
        if( v )
          cb->setChecked( v->m_Data.m_Bool );

        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, cb );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( cb, SIGNAL( stateChanged( int ) ), conn,
          SLOT( checkBoxChanged( int ) ) );
        e = cb_cont;
      }
      break;
    case E_VART_FLOAT:
      {
        tl = new QLabel( tr( "float" ) );

        QLineEdit* le = new QLineEdit;
        le->setFrame( false );
        le->setTextMargins( 5, 0, 0, 0 );
        e = le;

        if( v )
          le->setText( QString( "%1" ).arg( as_float( *v ) ) );
        le->setValidator( new QDoubleValidator( 0x0 ) );

        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, le );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( le, SIGNAL( editingFinished() ), conn, SLOT( lineEditChanged() ) );

      }
      break;
    case E_VART_INTEGER:
      {
        tl = new QLabel( tr( "integer" ) );

        QLineEdit* le = new QLineEdit;
        le->setFrame( false );
        le->setTextMargins( 5, 0, 0, 0 );
        e = le;

        if( v )
          le->setText( QString( "%1" ).arg( as_integer( *v ) ) );
        le->setValidator( new QIntValidator( 0x0 ) );

        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, le );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( le, SIGNAL( editingFinished() ), conn, SLOT( lineEditChanged() ) );

      }
      break;
    case E_VART_HASH:
      if( v && v->m_Type != E_VART_STRING )
      {
        v->m_Type = E_VART_STRING;
        v->m_Data.m_String.m_Parsed = 0x0;
        v->m_Data.m_String.m_Raw = 0x0;
      }
    case E_VART_STRING:
      {
        if( it->m_Type == E_VART_STRING )
          tl = new QLabel( tr( "string" ) );
        else if( it->m_Type == E_VART_HASH )
          tl = new QLabel( tr( "hash" ) );

        QLineEdit* le = new QLineEdit;
        le->setFrame( false );
        le->setTextMargins( 5, 0, 0, 0 );

        e = le;

        if( v )
        {
          le->setText( as_string( *v )->m_Raw );
          le->setCursorPosition( 0 );
        }

        ParamConnectorPlug* conn = new ParamConnectorPlug( it->m_Id.m_Hash, le );
        connect( conn, SIGNAL( dataChanged( QObject*, hash_t ) ), this,
          SLOT( paramChanged( QObject*, hash_t ) ) );
        connect( le, SIGNAL( editingFinished() ), conn, SLOT( lineEditChanged() ) );

      }
      break;
    case E_VART_UNDEFINED:
    case E_MAX_VARIABLE_TYPE:
      // Warning Killers
      break;
    }

    int row = tw->rowCount();

    l->setIndent( 5 );
    tl->setIndent( 5 );

    tw->setRowCount( row + 1 );
    tw->setCellWidget( row, 0, l );
    tw->setCellWidget( row, 1, tl );
    tw->setCellWidget( row, 2, e );

    it = it->m_Next;
  }

  tw->resizeRowsToContents();
  tw->setSelectionMode( QAbstractItemView::NoSelection );
  {
    QHeaderView* hw = tw->horizontalHeader();
    hw->setMinimumSectionSize( 65 );
    hw->setResizeMode( QHeaderView::ResizeToContents );
    hw->setStretchLastSection( true );
    hw->setHighlightSections( false );
    hw->setClickable( false );
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

  if( m_Node->m_Pare.m_Node != m_BeforeRelinkage.m_Pare.m_Node
      || m_Node->m_Pare.m_Tree != m_BeforeRelinkage.m_Pare.m_Tree
      || m_Node->m_Pare.m_Type != m_BeforeRelinkage.m_Pare.m_Type
      || m_Node->m_Prev != m_BeforeRelinkage.m_Prev || m_Node->m_Next
      != m_BeforeRelinkage.m_Next )
    signalModified( false );

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

      BehaviorTree* parent_tree = find_parent_tree( p->m_Pare );
      if( contains_reference_to_tree( m_Node, parent_tree ) )
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

      if( contains_reference_to_tree( m_Node, p ) )
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

