#include "btree_callbacks.h"
#include <btree/btree_func.h>
#include <malloc.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

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

void parser_error( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( pi )
  {
    QString fileName = pi->m_FileName->absoluteFilePath();
    std::string std_fileName = fileName.toStdString();
    fprintf( stdout, "%s(%d) : error : %s\n", std_fileName.c_str(),
      get_line_no( pc ), msg );
  }
  else
  {
    fprintf( stdout, "<no file>(%d) : error : %s\n",
      get_line_no( pc ), msg );
  }
}

void parser_warning( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)get_extra( pc );
  if( pi )
  {
    QString fileName = pi->m_FileName->absoluteFilePath();
    std::string std_fileName = fileName.toStdString();
    fprintf( stdout, "%s(%d) : warning : %s\n", std_fileName.c_str(),
      get_line_no( pc ), msg );
  }
  else
  {
    fprintf( stdout, "<no file>(%d) : warning : %s\n", get_line_no(
      pc ), msg );
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
