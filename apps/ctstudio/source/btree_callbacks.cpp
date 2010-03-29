#include "btree_callbacks.h"
#include <btree/btree_func.h>
#include <malloc.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>

#include <string>

int read_file( ParserContext pc, char* buffer, int maxsize )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( !pi )
    return 0;
  if( pi->m_File->atEnd() )
    return 0;
  return (int)pi->m_File->read( buffer, maxsize );
}

void parser_error( ParserContext pc, const char* msg_in )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  QString title( "Parser Error" );
  if( pi )
  {
    QString msg;
    msg = QString( "%1(%2) : error : %3" )
        .arg( pi->m_FileName->absoluteFilePath() )
        .arg( get_line_no( pc ) )
        .arg( msg_in );
    QMessageBox::critical( 0x0, title, msg );
  }
  else
  {
    QString msg;
    msg = QString( "<no file>(%1) : error : %2" )
        .arg( get_line_no( pc ) )
        .arg( msg_in );
    QMessageBox::critical( 0x0, title, msg );
  }
}

void parser_warning( ParserContext pc, const char* msg_in )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  QString title( "Parser Warning" );
  if( pi )
  {
    QString msg;
    msg = QString( "%1(%2) : warning : %3" )
        .arg( pi->m_FileName->absoluteFilePath() )
        .arg( get_line_no( pc ) )
        .arg( msg_in );
    QMessageBox::warning( 0x0, title, msg );
  }
  else
  {
    QString msg;
    msg = QString( "<no file>(%1) : warning : %2" )
        .arg( get_line_no( pc ) )
        .arg( msg_in );
    QMessageBox::warning( 0x0, title, msg );
  }
}

const char* parser_translate_include( ParserContext pc, const char* include )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( !pi )
    return include;
  QDir parent_dir( pi->m_FileName->absoluteDir() );
  QString qstr = parent_dir.absoluteFilePath( QString( include ) );
  std::string stdstr( qstr.toStdString() );
  return register_string( get_bt_context( pc ), stdstr.c_str() );
}

void write_file( SaverContext sc, const char* data, int size )
{
  SavingInfo* si = (SavingInfo*)get_extra( sc );
  si->m_File->write( data, size );
}

const char* saver_translate_include( SaverContext sc, const char* include )
{
  SavingInfo* si = (SavingInfo*)get_extra( sc );
  if( !si )
    return include;
  QDir parent_dir( si->m_FileName->absoluteDir() );
  QString qstr = parent_dir.relativeFilePath( QString( include ) );
  std::string stdstr( qstr.toStdString() );
  return register_string( get_bt_context( sc ), stdstr.c_str() );

}

void* allocate_memory( mem_size_t size )
{
  return malloc( size );
}

void free_memory( void* ptr )
{
  if( ptr )
    free( ptr );
}
