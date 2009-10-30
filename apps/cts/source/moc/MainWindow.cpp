/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "MainWindow.h"
#include "BehaviorTreeScene.h"
#include "BehaviorTreeView.h"

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

MainWindow::MainWindow()
	: m_BTreeView( 0x0 )
	, m_BTreeScene( 0x0 )
{
	setupUi(this);

	m_BTreeScene = new BehaviorTreeScene;
	m_BTreeView  = new BehaviorTreeView( m_BTreeScene );

	QGLFormat format( QGL::SampleBuffers );
	QGLWidget* gl_widget = new QGLWidget( format );

	if( gl_widget->isValid() )
	{
		m_BTreeView->setViewport( gl_widget );
		m_BTreeView->setRenderHint(QPainter::Antialiasing, true);
	}
	else
		delete gl_widget;

	setCentralWidget( m_BTreeView );

	connect( m_ActionOpen, SIGNAL(triggered()), this, SLOT(open()) );
	connect( m_ActionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()) );
	connect( m_ActionLayout, SIGNAL(triggered()), m_BTreeScene, SLOT(layoutNodes()) );

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
	restoreGeometry(settings.value("geometry").toByteArray());
}

void MainWindow::writeSettings()
{
	QSettings settings("calltree", "Call Tree Studio");
	settings.setValue("geometry", saveGeometry());
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

