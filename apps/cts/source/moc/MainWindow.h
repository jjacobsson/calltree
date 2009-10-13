#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "../../ui/ui_MainWindow.h"
#include <QtGui/QMainWindow>
#include <QtCore/QString>

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

	void open();

private:

	void setupStatusBar();

	bool okToContinue();
	void readSettings();
	void writeSettings();
	bool loadFile( const QString& fileName );

	void setCurrentFile( const QString& fileName );
	QString strippedName( const QString& fullFileName );

	BehaviorTreeWidget*	m_BTree;
};

#endif
