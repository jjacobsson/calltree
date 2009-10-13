#include "BehaviorTreeWidget.h"
#include <btree/btree.h>

#include <QtGui/QGraphicsScene>
#include <QtSvg/QGraphicsSvgItem>

BehaviorTreeWidget::BehaviorTreeWidget()
{
	const char* node_paths[] = {
	    ":/nodes/sequence.svg",
	    ":/nodes/selector.svg",
	    ":/nodes/parallel.svg",
	    ":/nodes/dyn_selector.svg",
	    ":/nodes/decorator.svg",
	    ":/nodes/action.svg"
	};
	QGraphicsSvgItem* item;
	for( int i = 0; i < 6; ++i )
	{
		item = new QGraphicsSvgItem( node_paths[i] );
		item->setPos( i * 256.0f, 0.0f );
		addItem( item );
	}
}

bool BehaviorTreeWidget::readFile( const QString& filename )
{
	BehaviorTree bt;
	int returnCode = bt.Parse( filename.toAscii() );
	if( returnCode != 0 )
		return false;

	//parseNode( bt.m_Root, 0x0 );

	return true;
}
/*
void BehaviorTreeWidget::parseNode( Node* n )
{
	while( n )
	{
		switch( n->m_Grist.m_Type )
		{
		case E_GRIST_SEQUENCE:
			parseNode( n->m_Grist.m_Sequence.m_FirstChild, twi );
			break;
		case E_GRIST_SELECTOR:
			parseNode( n->m_Grist.m_Selector.m_FirstChild, twi );
			break;
		case E_GRIST_PARALLEL:
			parseNode( n->m_Grist.m_Parallel.m_FirstChild, twi );
			break;
		case E_GRIST_DYN_SELECTOR:
			parseNode( n->m_Grist.m_DynSelector.m_FirstChild, twi );
			break;
		case E_GRIST_DECORATOR:
			parseNode( n->m_Grist.m_Decorator.m_Child, twi );
			break;
		case E_GRIST_ACTION:
			break;
		}
		n = n->m_Sibling;
	}
}
*/
