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

#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtSvg/QSvgRenderer>

#include <other/getopt.h>

char* g_inputFileName = 0x0;
char* g_outputFileName = 0x0;

int main(int argc, char *argv[])
{
    GetOptContext ctx;
    init_getopt_context( &ctx );
    char c;

    while( (c = getopt( argc, argv, "i:o:", &ctx )) != -1 )
    {
      switch( c )
      {
      case 'i':
        g_inputFileName = ctx.optarg;
        break;
      case 'o':
        g_outputFileName = ctx.optarg;
        break;
      }
    }

    if( !g_inputFileName || !g_outputFileName )
    {
      printf( "error: you must specify input and output file's with -i & -o\n" );
      return -1;
    }

    QApplication app( argc, argv );

    QSvgRenderer svgr( QString( g_inputFileName ), 0x0 );
    if( !svgr.isValid() )
    {
      printf( "error: failed to create renderer for input file %s\n", g_inputFileName );
      return -1;
    }

    QImage img( 64, 64, QImage::Format_ARGB32_Premultiplied );
    img.fill( 0 );
    QPainter p( &img );
    svgr.render( &p );
    p.end();

    QFileInfo fi( g_outputFileName );
    QDir dir;
    if( !dir.mkpath( fi.absolutePath() ) )
    {
      printf( "error: unable to create output directory %s\n", fi.absolutePath().toAscii().constData() );
      return -1;
    }

    if( !img.save( g_outputFileName, "PNG" ) )
    {
      printf( "error: failed to save output file %s\n", g_outputFileName );
      return -1;
    }

    return 0;
}

