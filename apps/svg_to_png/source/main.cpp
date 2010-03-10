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
#include <QtGui/QPixmap>
#include <QtGui/QImage>

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

    QPixmap pixmap(64,64);
    pixmap = QPixmap(g_inputFileName, "SVG").scaled(
      64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
    );

    if( pixmap.isNull() )
    {
      printf( "error: failed to load input file %s\n", g_inputFileName );
      return -1;
    }

    if( !pixmap.save( g_outputFileName, "PNG" ) )
    {
      printf( "error: failed to save output file %s\n", g_outputFileName );
      return -1;
    }

    return 0;
}

