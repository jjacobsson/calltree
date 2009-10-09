#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

#include "../ui/ui_main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	Ui::MainWindow ui;
	QMainWindow* window = new QMainWindow;
	ui.setupUi( window );
    window->show();

    return app.exec();
}
