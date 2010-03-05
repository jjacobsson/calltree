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

MainWindow::MainWindow() :
  m_BTreeView( 0x0 ), m_BTreeScene( 0x0 )
{
  setupUi( this );

  m_BTreeScene = new BehaviorTreeScene( this );
  m_BTreeView = new BehaviorTreeView( m_BTreeScene );
  m_List = new BehaviorTreeList;

  m_NodesDock->setWidget( m_List );

  setCentralWidget( m_BTreeView );

  setupStatusBar();
  setupActions();
  setupMenus();

  readSettings();

  setCurrentFile( "" );

  connect( m_ActionOpen, SIGNAL(triggered()), this, SLOT(open()) );
  connect( m_ActionSave, SIGNAL(triggered()), this, SLOT(save()) );
  connect( m_ActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()) );
  connect( m_ActionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()) );
  connect( m_BTreeView, SIGNAL( statusMessage( QString, int ) ), this->statusBar(), SLOT( showMessage( QString, int ) ) );
  connect( m_BTreeScene, SIGNAL( modified() ), this, SLOT(treeModified()) );
  connect( m_BTreeScene, SIGNAL( updatedSymbols( BehaviorTreeContext ) ), m_List, SLOT( loadSymbols( BehaviorTreeContext ) ) );
  connect( m_BTreeScene, SIGNAL( itemSelected( QWidget* ) ), this, SLOT( setPropertyWidget( QWidget* ) ) );
  connect( m_ExitAction, SIGNAL( triggered()), this, SLOT(close()) );
  connect( m_ActionDelete, SIGNAL( triggered() ), m_BTreeScene, SLOT( deleteSelected() ) );

}

void MainWindow::open()
{
  if( okToContinue() )
  {
    QString fileName = QFileDialog::getOpenFileName( this, tr(
      "Open Behavior Tree" ), ".", tr( "Behavior Tree files (*.bts)" ) );
    if( !fileName.isEmpty() )
      loadFile( fileName );
  }
}

bool MainWindow::save()
{
  if( m_CurrentFile.isEmpty())
    return saveAs();
  else
    return saveFile(m_CurrentFile);
  return false;
}

bool MainWindow::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName(this,
    tr("Save Behavior Tree"), ".",
    tr("Behavior Tree files (*.bts)"));
  if( fileName.isEmpty() )
    return false;
  return saveFile( fileName );
}

void MainWindow::openRecentFile()
{
  if( okToContinue() )
  {
    QAction* action = qobject_cast<QAction*>(sender());
    if( action )
      loadFile(action->data().toString());
  }
}

void MainWindow::treeModified()
{
  setWindowModified( true );
}

void MainWindow::setPropertyWidget( QWidget* property_widget )
{
  m_PropertiesDock->setWidget( property_widget );
}

void MainWindow::setupStatusBar()
{
}

void MainWindow::setupActions()
{
  for( int i = 0; i < MaxRecentFiles; ++i )
  {
    m_RecentFileActions[i] = new QAction( this );
    m_RecentFileActions[i]->setVisible( false );
    connect( m_RecentFileActions[i], SIGNAL(triggered()),
      this, SLOT(openRecentFile()) );
  }

  m_ExitAction = new QAction( tr("&Exit"), this );
  m_ExitAction->setShortcut(tr("Ctrl+Q"));
}

void MainWindow::setupMenus()
{
  m_SeparatorAction = m_MenuFile->addSeparator();
  m_SeparatorAction->setVisible(false);
  for( int i = 0; i < MaxRecentFiles; ++i )
    m_MenuFile->addAction(m_RecentFileActions[i]);
  m_MenuFile->addSeparator();
  m_MenuFile->addAction( m_ExitAction );
}

void MainWindow::updateRecentFileActions()
{
  QMutableStringListIterator i( m_RecentFiles );
  while( i.hasNext() )
  {
    if( !QFile::exists( i.next() ) )
      i.remove();
  }
  for( int j = 0; j < MaxRecentFiles; ++j )
  {
    if( j < m_RecentFiles.count() )
    {
      QString text = tr( "&%1 %2" ) .arg( j + 1 ) .arg( strippedName(
        m_RecentFiles[j] ) );
      m_RecentFileActions[j]->setText( text );
      m_RecentFileActions[j]->setData( m_RecentFiles[j] );
      m_RecentFileActions[j]->setVisible( true );
    }
    else
    {
      m_RecentFileActions[j]->setVisible( false );
    }
  }
  m_SeparatorAction->setVisible( !m_RecentFiles.isEmpty() );
}

void MainWindow::closeEvent( QCloseEvent* event )
{
  if( okToContinue() )
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
  QSettings settings( "calltree", "Call Tree Studio" );
  settings.beginGroup( "main window" );
  restoreState( settings.value( "state" ).toByteArray() );
  restoreGeometry( settings.value( "geometry" ).toByteArray() );
  m_RecentFiles = settings.value("recentFiles").toStringList();
  settings.endGroup();
  updateRecentFileActions();
}

void MainWindow::writeSettings()
{
  QSettings settings( "calltree", "Call Tree Studio" );
  settings.beginGroup( "main window" );
  settings.setValue( "state", saveState() );
  settings.setValue( "geometry", saveGeometry() );
  settings.setValue( "recentFiles", m_RecentFiles );
  settings.endGroup();
}

bool MainWindow::loadFile( const QString& fileName )
{
  if( m_BTreeScene->readFile( fileName ) )
  {
    QRectF itemsRect = m_BTreeScene->itemsBoundingRect();
    m_BTreeScene->setSceneRect( itemsRect );
    m_BTreeView->fitInView( itemsRect, Qt::KeepAspectRatio );

    setCurrentFile( fileName );
    statusBar()->showMessage( tr( "File loaded" ), 2000 );
    return true;
  }
  statusBar()->showMessage( tr( "Load Failed!" ), 2000 );
  return false;
}

bool MainWindow::saveFile( const QString& fileName )
{
  if( !m_BTreeScene->writeFile( fileName ) )
  {
    statusBar()->showMessage(tr("Saving canceled"), 2000 );
    return false;
  }
  setCurrentFile( fileName );
  statusBar()->showMessage(tr("File saved"), 2000 );
  return true;
}

void MainWindow::setCurrentFile( const QString& fileName )
{
  m_CurrentFile = fileName;
  setWindowModified( false );
  QString shownName = "Untitled";
  if( !m_CurrentFile.isEmpty() )
  {
    shownName = strippedName( m_CurrentFile );
    m_RecentFiles.removeAll(m_CurrentFile);
    m_RecentFiles.prepend(m_CurrentFile);
    updateRecentFileActions();
  }
  setWindowTitle( tr( "%1[*] - %2" ).arg( shownName ).arg( tr(
    "Call Tree Studio" ) ) );
}

QString MainWindow::strippedName( const QString &fullFileName )
{
  return QFileInfo( fullFileName ).fileName();
}

bool MainWindow::okToContinue()
{
  if( isWindowModified() )
  {
    int r = QMessageBox::warning( this, tr( "Calltree Studio" ), tr(
      "The behavior tree has been modified.\n"
        "Do you want to save your changes?" ), QMessageBox::Yes
        | QMessageBox::Default, QMessageBox::No, QMessageBox::Cancel
        | QMessageBox::Escape );
    if( r == QMessageBox::Yes )
    {
      return save();
    }
    else if( r == QMessageBox::Cancel )
    {
      return false;
    }
  }
  return true;
}

