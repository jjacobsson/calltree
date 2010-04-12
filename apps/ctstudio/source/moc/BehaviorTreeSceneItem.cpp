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

#include "BehaviorTreeSceneItem.h"
#include "../NodeToNodeArrow.h"
#include "../standard_resources.h"
#include "../SvgCache.h"
#include <btree/btree.h>

#include <QtGui/QtGui>
#include <QtSvg/QGraphicsSvgItem>

const qreal g_IconScale = 1.0 / 3.0;
const qreal g_IconPad   = 16.0;

BehaviorTreeSceneItem::BehaviorTreeSceneItem( BehaviorTreeContext ctx, QGraphicsObject* parent )
  : QGraphicsObject( parent )
  , m_MouseState( E_MS_NONE )
  , m_Graphics( 0x0 )
  , m_PropertyWidget( 0x0 )
  , m_Context( ctx )
{
  setFlag( QGraphicsItem::ItemIsMovable, true );
  setFlag( QGraphicsItem::ItemIsSelectable, true );
  setFlag( QGraphicsItem::ItemStacksBehindParent, false );
  setFlag( QGraphicsItem::ItemSendsGeometryChanges, true );

  setZValue( 0.0 );

  for( int i = 0; i < ICON_COUNT; ++i )
  {
    m_Icons[i] = new QGraphicsSvgItem( this );
    m_Icons[i]->setSharedRenderer( SvgCache::get( g_IconNames[i] ) );
    m_Icons[i]->setScale( g_IconScale );
    m_Icons[i]->setZValue( 10.0 );
    m_Icons[i]->setVisible( false );
  }
}

BehaviorTreeSceneItem::~BehaviorTreeSceneItem()
{
  removeArrows();
  delete m_PropertyWidget;
  m_PropertyWidget = 0x0;

  for( int i = 0; i < ICON_COUNT; ++i )
  {
    delete m_Icons[i];
    m_Icons[i] = 0x0;
  }

  emit itemDeleted();
}

void BehaviorTreeSceneItem::removeArrow(NodeToNodeArrow *arrow)
{
    int index = m_Arrows.indexOf(arrow);

    if (index != -1)
        m_Arrows.removeAt(index);
}

void BehaviorTreeSceneItem::removeArrows()
{
  foreach( NodeToNodeArrow *arrow, m_Arrows )
  {
    arrow->startItem()->removeArrow(arrow);
    arrow->endItem()->removeArrow(arrow);
    scene()->removeItem(arrow);
    delete arrow;
  }
}

void BehaviorTreeSceneItem::addArrow(NodeToNodeArrow *arrow)
{
  m_Arrows.append(arrow);
}

NodeToNodeArrow* BehaviorTreeSceneItem::findArrowTo( BehaviorTreeSceneItem* other )
{
  foreach( NodeToNodeArrow *arrow, m_Arrows )
  {
    if( arrow->startItem() == this && arrow->endItem() == other )
      return arrow;
    if( arrow->startItem() == other && arrow->endItem() == this )
      return arrow;
  }
  return 0x0;
}

bool BehaviorTreeSceneItem::toCloseForArrow( const BehaviorTreeSceneItem* other ) const
{
  const QGraphicsItem* i1 = this;
  const QGraphicsItem* i2 = other;
  if( m_Graphics )
    i1 = m_Graphics;
  if( other->m_Graphics )
    i2 = other->m_Graphics;
  return i1->collidesWithItem( i2 );
}

void BehaviorTreeSceneItem::positionArrows()
{
  foreach( NodeToNodeArrow *arrow, m_Arrows )
  {
    arrow->updatePosition();
  }
  QList<QGraphicsItem*> children( childItems() );
  foreach( QGraphicsItem* gitem, children )
  {
    BehaviorTreeSceneItem* item = qgraphicsitem_cast<BehaviorTreeSceneItem*>( gitem );
    if( !item )
      continue;
    item->positionArrows();
  }
}

QPointF BehaviorTreeSceneItem::arrowAnchor() const
{
  QPointF r( scenePos() );
  if( m_Graphics )
  {
    QRectF  br( m_Graphics->boundingRect() );
    QPointF p( m_Graphics->scenePos() );
    p.rx() += br.width() / 2.0;
    p.ry() += br.height() / 2.0;
    r = p;
  }
  return r;
}

QPointF BehaviorTreeSceneItem::iconPosition() const
{
  return QPointF( 0, 0 );
}

QRectF BehaviorTreeSceneItem::layoutBoundingRect() const
{
  return boundingRect();
}
qreal BehaviorTreeSceneItem::layoutOffset() const
{
  return 0.0;
}
BehaviorTreeSceneItem* BehaviorTreeSceneItem::getParent()
{
  return 0x0;
}
BehaviorTreeSceneItem* BehaviorTreeSceneItem::firstChild()
{
  return 0x0;
}
BehaviorTreeSceneItem* BehaviorTreeSceneItem::nextSibling()
{
  return 0x0;
}
bool BehaviorTreeSceneItem::validForDrop() const
{
  return true;
}

void BehaviorTreeSceneItem::dragMove()
{
}

void BehaviorTreeSceneItem::dragBegin()
{
}

void BehaviorTreeSceneItem::dragEnd()
{
  emit itemDragged();
}

void BehaviorTreeSceneItem::dragFail()
{

}

void BehaviorTreeSceneItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
  if(!(option->state & QStyle::State_Selected))
    return;

  QRectF brect( boundingRect() );
  const QRectF mbrect = painter->transform().mapRect(brect);
  if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
      return;

  qreal itemPenWidth = 1;
  qreal penWidth = 0;
  const qreal pad = itemPenWidth / 2;
  const QColor fgcolor = option->palette.windowText().color();
  const QColor bgcolor( // ensure good contrast against fgcolor
      fgcolor.red()   > 127 ? 0 : 255,
      fgcolor.green() > 127 ? 0 : 255,
      fgcolor.blue()  > 127 ? 0 : 255);

  painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(brect.adjusted(pad, pad, -pad, -pad));

  painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(brect.adjusted(pad, pad, -pad, -pad));
}

void BehaviorTreeSceneItem::deleteThis()
{
  delete this;
}

void BehaviorTreeSceneItem::positionIcons()
{
  QPointF start_pos( iconPosition() );
  QRectF rect;
  for( int i = 0; i < ICON_COUNT; ++i )
  {
    if( !m_Icons[i]->isVisible() )
      continue;

    m_Icons[i]->setPos( start_pos );
    rect = m_Icons[i]->boundingRect();
    start_pos.rx() += (rect.width() * g_IconScale) + g_IconPad;
  }
}

QVariant BehaviorTreeSceneItem::itemChange( GraphicsItemChange change,
  const QVariant &value )
{
  switch( change )
  {
  case ItemSelectedChange:
    update();
    if( value.toBool() )
      emit itemSelected( m_PropertyWidget );
    else
      emit itemSelected( 0x0 );
    break;
  case ItemPositionHasChanged:
    positionArrows();
    break;
  default:
    break;
  }
  return QGraphicsItem::itemChange( change, value );
}

void BehaviorTreeSceneItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
    m_MouseState = E_MS_LB_DOWN;
    m_StartPos = event->screenPos();
  }
  QGraphicsItem::mousePressEvent( event );
}

void BehaviorTreeSceneItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
    if( m_MouseState == E_MS_DRAGGING )
      dragEnd();

    m_MouseState = E_MS_NONE;
  }
  QGraphicsItem::mouseReleaseEvent( event );
}

void BehaviorTreeSceneItem::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
  if( m_MouseState == E_MS_LB_DOWN )
  {
    if( m_StartPos != event->screenPos() )
    {
      dragBegin();
      m_MouseState = E_MS_DRAGGING;
    }
  }
  if( m_MouseState == E_MS_DRAGGING )
    dragMove();

  QGraphicsItem::mouseMoveEvent( event );
}

void BehaviorTreeSceneItem::signalModified( bool geometry_changed )
{
  emit modified(geometry_changed);
}
