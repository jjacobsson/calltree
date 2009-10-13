#include "BehaviorTreeWidget.h"
#include <btree/btree.h>

BehaviorTreeWidget::BehaviorTreeWidget()
{
	QStringList headers;
	headers << tr("Node");
	headers << tr("Type");
	setHeaderLabels( headers );
}

bool BehaviorTreeWidget::readFile( const QString& filename )
{
	BehaviorTree bt;
	int returnCode = bt.Parse( filename.toAscii() );
	if( returnCode != 0 )
		return false;

	clear();

	parseNode( bt.m_Root, 0x0 );

	return true;
}

void BehaviorTreeWidget::parseNode( Node* n, QTreeWidgetItem* parent )
{
	while( n )
	{
		QStringList strings;
		strings << n->m_Id.m_Text;
		QTreeWidgetItem* twi = 0x0;

		switch( n->m_Grist.m_Type )
		{
		case E_GRIST_SEQUENCE:
			strings << tr("Sequence");
			twi = new QTreeWidgetItem( strings );
			twi->setIcon( 0, QIcon(":/icons/nodes/sequence.png") );
			parseNode( n->m_Grist.m_Sequence.m_FirstChild, twi );
			break;
		case E_GRIST_SELECTOR:
			strings << tr("Selector");
			twi = new QTreeWidgetItem( strings );
			twi->setIcon( 0, QIcon(":/icons/nodes/selector.png") );
			parseNode( n->m_Grist.m_Selector.m_FirstChild, twi );
			break;
		case E_GRIST_PARALLEL:
			strings << tr("Parallel");
			twi = new QTreeWidgetItem( strings );
			twi->setIcon( 0, QIcon(":/icons/nodes/parallel.png") );
			parseNode( n->m_Grist.m_Parallel.m_FirstChild, twi );
			break;
		case E_GRIST_DYN_SELECTOR:
			strings << tr("Dynamic Selector");
			twi = new QTreeWidgetItem( strings );
			twi->setIcon( 0, QIcon(":/icons/nodes/dyn_selector.png") );
			parseNode( n->m_Grist.m_DynSelector.m_FirstChild, twi );
			break;
		case E_GRIST_DECORATOR:
			strings << tr("Decorator");
			twi = new QTreeWidgetItem( strings );
			twi->setIcon( 0, QIcon(":/icons/nodes/decorator.png") );
			parseNode( n->m_Grist.m_Decorator.m_Child, twi );
			break;
		case E_GRIST_ACTION:
			strings << tr("Action");
			twi = new QTreeWidgetItem( strings );
			twi->setIcon( 0, QIcon(":/icons/nodes/action.png") );
			break;
		}

		if( parent )
			parent->addChild( twi );
		else
			addTopLevelItem( twi );

		n = n->m_Sibling;

	}
}
