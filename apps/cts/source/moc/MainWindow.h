/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "../../ui/ui_MainWindow.h"
#include <QtGui/QMainWindow>
#include <QtCore/QString>

// Internal Forward declares
class BehaviorTreeScene;
class BehaviorTreeView;
class BehaviorTreeList;

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

	BehaviorTreeList*   m_List;
	BehaviorTreeView*	m_BTreeView;
	BehaviorTreeScene*	m_BTreeScene;
};

#endif
