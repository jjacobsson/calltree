/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "MainWindow.h"
#include "BehaviorTreeWidget.h"

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

MainWindow::MainWindow()
	: m_BTreeView( 0x0 )
	, m_BTree(0x0)
{
	setupUi(this);

	m_BTree     = new BehaviorTreeWidget;
	m_BTreeView = new QGraphicsView( m_BTree );
	m_BTreeView->setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
	m_BTreeView->setDragMode( QGraphicsView::ScrollHandDrag );
	m_BTreeView->setResizeAnchor( QGraphicsView::AnchorUnderMouse );

	QGLFormat format;
	QGLWidget* view_port = new QGLWidget( format );

	if( view_port->isValid() )
	{
		m_BTreeView->setViewport( view_port );
		setCentralWidget( view_port );
	}
	else
	{
		delete view_port;
		setCentralWidget( m_BTreeView );
	}
	connect(m_ActionOpen, SIGNAL(triggered()), this, SLOT(open()));
	connect(m_ActionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

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
	QRect rect = settings.value("geometry", QRect(0, 0, 800, 600)).toRect();
	move(rect.topLeft());
	resize(rect.size());
}

void MainWindow::writeSettings()
{
	QSettings settings("calltree", "Call Tree Studio");
	settings.setValue("geometry", geometry());
}

bool MainWindow::loadFile(const QString& fileName)
{
	if( m_BTree->readFile( fileName ) )
	{
		m_BTreeView->fitInView( m_BTree->sceneRect(), Qt::KeepAspectRatio );

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

