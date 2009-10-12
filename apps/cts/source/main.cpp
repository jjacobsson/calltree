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
