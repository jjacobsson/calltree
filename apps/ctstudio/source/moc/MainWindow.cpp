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

#include "MainWindow.h"
#include "BehaviorTreeScene.h"
#include "BehaviorTreeView.h"
#include "BehaviorTreeList.h"

#include <QtGui/QtGui>

MainWindow::MainWindow()
	: m_BTreeView( 0x0 )
	, m_BTreeScene( 0x0 )
{
	setupUi(this);

	m_BTreeScene = new BehaviorTreeScene;
	m_BTreeView  = new BehaviorTreeView( m_BTreeScene );
	m_List = new BehaviorTreeList;

	m_Dock->setWidget( m_List );

	setCentralWidget( m_BTreeView );

	connect( m_ActionOpen, SIGNAL(triggered()), this, SLOT(open()) );
	connect( m_ActionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()) );

	setupStatusBar();

	readSettings();

	setCurrentFile("");
}

void MainWindow::open()
{
	if (okToContinue())
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr(
				"Open Behavior Tree"), ".", tr("Behavior Tree files (*.bts)"));
		if (!fileName.isEmpty())
			loadFile(fileName);
	}
}

void MainWindow::setupStatusBar()
{
	m_Statusbar->addWidget(m_StatusLabelCurrentFile);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (okToContinue())
	{
		writeSettings();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void MainWindow::readSettings()
{
	QSettings settings("calltree", "Call Tree Studio");
	settings.beginGroup( "main window" );
	restoreState( settings.value("state").toByteArray() );
	restoreGeometry( settings.value("geometry").toByteArray() );
	settings.endGroup();
}

void MainWindow::writeSettings()
{
	QSettings settings("calltree", "Call Tree Studio");
	settings.beginGroup( "main window" );
	settings.setValue( "state", saveState() );
	settings.setValue( "geometry", saveGeometry() );
	settings.endGroup();
}

bool MainWindow::loadFile(const QString& fileName)
{
	if( m_BTreeScene->readFile( fileName ) )
	{
		QRectF itemsRect =  m_BTreeScene->itemsBoundingRect();
		m_BTreeScene->setSceneRect( itemsRect );
		m_BTreeView->fitInView( itemsRect, Qt::KeepAspectRatio );

		setCurrentFile(fileName);
		statusBar()->showMessage(tr("File loaded"), 2000);
		return true;
	}
	statusBar()->showMessage(tr("Load Failed!"), 2000);
	return false;
}

void MainWindow::setCurrentFile( const QString& fileName )
{
	QString shownName = "Untitled";
	if( !fileName.isEmpty() )
	{
		shownName = strippedName(fileName);
		//recentFiles.removeAll(m_CurrentFile);
		//recentFiles.prepend(m_CurrentFile);
		//updateRecentFileActions();
	}
	setWindowTitle( tr("%1[*] - %2").arg(shownName).arg(tr("Call Tree Studio") ) );
	m_StatusLabelCurrentFile->setText( fileName );
}

QString MainWindow::strippedName( const QString &fullFileName )
{
	return QFileInfo(fullFileName).fileName();
}

bool MainWindow::okToContinue()
{
	return true;
}

