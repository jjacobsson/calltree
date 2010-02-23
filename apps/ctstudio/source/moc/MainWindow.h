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
	bool save();
	bool saveAs();

private:

	void setupStatusBar();

	bool okToContinue();
	void readSettings();
	void writeSettings();
	bool loadFile( const QString& fileName );
	bool saveFile( const QString& fileName );

	void setCurrentFile( const QString& fileName );
	QString strippedName( const QString& fullFileName );

	BehaviorTreeList*   m_List;
	BehaviorTreeView*	m_BTreeView;
	BehaviorTreeScene*	m_BTreeScene;

	QString m_CurrentFile;
};

#endif
