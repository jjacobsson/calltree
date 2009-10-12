#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "../../ui/ui_MainWindow.h"
#include <QtGui/QMainWindow>

#include <btree/btree.h>

class BehaviorTreeWidget;
class QGraphicsView;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:

	void loadFile();

private:

	void readSettings();
	void writeSettings();

	BehaviorTreeWidget*	m_BTreeWidget;
	QGraphicsView*		m_View;
};

#endif
