#ifndef BEHAVOIRTREEWIDGET_H_INCLUDED
#define BEHAVOIRTREEWIDGET_H_INCLUDED

#include <QtGui/QTreeWidget>

class QString;
class QTreeWidgetItem;
struct Node;

class BehaviorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	BehaviorTreeWidget();

	bool readFile( const QString& fileName );

private:
	void parseNode( Node*, QTreeWidgetItem* );
};

#endif
