#ifndef BEHAVOIRTREEWIDGET_H_INCLUDED
#define BEHAVOIRTREEWIDGET_H_INCLUDED

#include <QtGui/QTreeWidget>

class BehaviorTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	BehaviorTreeWidget();

	bool readFile( QString* fileName );

};

#endif
