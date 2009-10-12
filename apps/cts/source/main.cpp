#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

#include "moc/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	MainWindow* window = new MainWindow;
	window->show();

    return app.exec();
}
