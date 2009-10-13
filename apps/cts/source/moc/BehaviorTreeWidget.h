#ifndef BEHAVOIRTREEWIDGET_H_INCLUDED
#define BEHAVOIRTREEWIDGET_H_INCLUDED

#include <QtGui/QGraphicsScene>

// Qt Forward declares
class QString;
class QGraphicsScene;

// BT Forward declares
struct Node;

class BehaviorTreeWidget : public QGraphicsScene
{
	Q_OBJECT
public:
	BehaviorTreeWidget();

	bool readFile( const QString& fileName );

private:

	//void parseNode( Node*, QTreeWidgetItem* );

	QGraphicsView*  m_View;
	QGraphicsScene* m_Scene;

};

#endif
