/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

#include "moc/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	MainWindow main_window;
	main_window.show();

    return app.exec();
}
