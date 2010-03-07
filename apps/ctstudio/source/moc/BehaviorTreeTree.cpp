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

#include "BehaviorTreeTree.h"
#include "../standard_resources.h"
#include <btree/btree.h>
#include <other/lookup3.h>

#include <QtCore/QRegExp>

#include <QtSvg/QGraphicsSvgItem>
#include <QtGui/QFont>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRegExpValidator>

class PreventDuplicateSymbols : public QRegExpValidator
{
public:
  PreventDuplicateSymbols( BehaviorTreeContext ctx, QObject* parent )
    : QRegExpValidator(  QRegExp( "([a-zA-Z_])([a-zA-Z0-9_])*"), parent )
    , m_Context( ctx )
  {}

  QValidator::State validate ( QString & input, int & pos ) const
  {
    QValidator::State s = QRegExpValidator::validate( input, pos );
    if( s == QValidator::Invalid )
      return s;
    hash_t hash = hashlittle( input.toAscii().constData() );
    NamedSymbol* ns = BehaviorTreeContextFindSymbol( m_Context, hash );
    if( ns )
      return QValidator::Intermediate;
    return s;
  }
private:
  BehaviorTreeContext m_Context;
};

BehaviorTreeTree::BehaviorTreeTree( BehaviorTreeContext ctx, BehaviorTree* tree )
  : BehaviorTreeSceneItem()
  , m_Context( ctx )
  , m_Tree( tree )
  , m_Label( 0x0 )
{
  setFlag( QGraphicsItem::ItemIsMovable, false );
  m_Graphics = new QGraphicsSvgItem( ":/nodes/tree.svg", this );
  setupLabel( m_Tree->m_Id.m_Text );
  setupPropertyEditor();
}

QRectF BehaviorTreeTree::boundingRect() const
{
  QRectF rect = m_Graphics->boundingRect().translated( m_Graphics->pos() );
  if( m_Label )
    rect |= m_Label->boundingRect().translated(m_Label->pos());
  return rect;
}

QRectF BehaviorTreeTree::layoutBoundingRect() const
{
  return m_Graphics->boundingRect().translated( m_Graphics->pos() );
}

qreal BehaviorTreeTree::layoutOffset() const
{
  return m_Graphics->pos().rx();
}

void BehaviorTreeTree::destroyResources( BehaviorTreeContext ctx )
{
  BehaviorTreeContextRemoveSymbol( ctx, m_Tree->m_Id.m_Hash );
  BehaviorTreeContextFreeObject( ctx, m_Tree );
  m_Tree = 0x0;
}

BehaviorTreeSceneItem* BehaviorTreeTree::firstChild()
{
  if( m_Tree->m_Root )
    return (BehaviorTreeSceneItem*)m_Tree->m_Root->m_UserData;
  return 0x0;
}

void BehaviorTreeTree::updateName()
{
  QLineEdit* le = qobject_cast<QLineEdit*>(sender());
  if( !le )
    return;

  if( strcmp( le->text().toAscii().constData(), m_Tree->m_Id.m_Text ) == 0 )
    return;

  BehaviorTreeContextRemoveSymbol( m_Context, m_Tree->m_Id.m_Hash );

  QByteArray new_str( le->text().toAscii() );
  m_Tree->m_Id.m_Text = BehaviorTreeContextRegisterString( m_Context, new_str.constData() );
  m_Tree->m_Id.m_Hash = hashlittle( new_str.constData() );

  NamedSymbol ns;
  ns.m_Type = E_ST_TREE;
  ns.m_Symbol.m_Tree = m_Tree;
  BehaviorTreeContextRegisterSymbol( m_Context, ns );

  setupLabel( m_Tree->m_Id.m_Text );

  signalModified();
}

void BehaviorTreeTree::setupLabel( const char* str )
{
  if( !str )
    return;

  delete m_Label;
  m_Label = 0x0;

  QFont font;
  font.setPixelSize(64);
  m_Label = new QGraphicsTextItem( str, this );
  m_Label->setFont( font );
  QPointF p;
  QRectF r( m_Label->boundingRect() );

  p.rx() = 0.0;

  float move_node = (r.width() - 256.0) / 2.0;
  if( move_node > 0 )
    m_Graphics->setPos( move_node, 0.0 );
  else
    p.rx() = -move_node;

  p.ry() = -r.height();
  m_Label->setPos( p );
}

void BehaviorTreeTree::setupPropertyEditor()
{
  m_PropertyWidget = new QWidget;
  QFormLayout* form = new QFormLayout( m_PropertyWidget );

  QLineEdit* edit = new QLineEdit( m_Tree->m_Id.m_Text );
  edit->setValidator( new PreventDuplicateSymbols( m_Context, edit ) );
  connect( edit, SIGNAL( returnPressed() ), this, SLOT( updateName() ) );

  form->addRow( new QLabel( tr( "Name" ) ), edit );
}

