#include <QtGui/QApplication>
#include <QtGui/QLabel>

int main( int argc, char* argv[] )
{
	QApplication app( argc, argv );
	QLabel* label = new QLabel( "Hello QT!" );
	label->show();
	return app.exec();
}
