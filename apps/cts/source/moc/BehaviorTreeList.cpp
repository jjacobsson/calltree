/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <QtGui/QtGui>

#include "BehaviorTreeList.h"

BehaviorTreeList::BehaviorTreeList(QWidget *parent)
     : QListWidget(parent)
 {
     setDragEnabled(true);
     setViewMode(QListView::IconMode);
     setIconSize(QSize(60, 60));
     setSpacing(10);
     setAcceptDrops(true);
     setDropIndicatorShown(true);
 }

 void BehaviorTreeList::dragEnterEvent(QDragEnterEvent *event)
 {
     if (event->mimeData()->hasFormat("image/x-puzzle-piece"))
         event->accept();
     else
         event->ignore();
 }

 void BehaviorTreeList::dragMoveEvent(QDragMoveEvent *event)
 {
     if (event->mimeData()->hasFormat("image/x-puzzle-piece")) {
         event->setDropAction(Qt::MoveAction);
         event->accept();
     } else
         event->ignore();
 }

 void BehaviorTreeList::dropEvent(QDropEvent *event)
 {
     if (event->mimeData()->hasFormat("image/x-puzzle-piece")) {
         QByteArray pieceData = event->mimeData()->data("image/x-puzzle-piece");
         QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
         QPixmap pixmap;
         QPoint location;
         dataStream >> pixmap >> location;

         addPiece(pixmap, location);

         event->setDropAction(Qt::MoveAction);
         event->accept();
     } else
         event->ignore();
 }

 void BehaviorTreeList::addPiece(QPixmap pixmap, QPoint location)
 {
     QListWidgetItem *pieceItem = new QListWidgetItem(this);
     pieceItem->setIcon(QIcon(pixmap));
     pieceItem->setData(Qt::UserRole, QVariant(pixmap));
     pieceItem->setData(Qt::UserRole+1, location);
     pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                         | Qt::ItemIsDragEnabled);
 }

 void BehaviorTreeList::startDrag(Qt::DropActions /*supportedActions*/)
 {
     QListWidgetItem *item = currentItem();

     QByteArray itemData;
     QDataStream dataStream(&itemData, QIODevice::WriteOnly);
     QPixmap pixmap = qVariantValue<QPixmap>(item->data(Qt::UserRole));
     QPoint location = item->data(Qt::UserRole+1).toPoint();

     dataStream << pixmap << location;

     QMimeData *mimeData = new QMimeData;
     mimeData->setData("image/x-puzzle-piece", itemData);

     QDrag *drag = new QDrag(this);
     drag->setMimeData(mimeData);
     drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
     drag->setPixmap(pixmap);

     if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
         delete takeItem(row(item));
 }
