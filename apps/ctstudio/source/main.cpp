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
#include <QtGui/QMainWindow>

#include "moc/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	MainWindow main_window;
	main_window.show();

    return app.exec();
}
