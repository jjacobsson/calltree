#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QPushButton>

#include "FindDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget *window = new QWidget;
    window->setWindowTitle("Enter Your Age");
    QSpinBox *spinBox = new QSpinBox;
    QSlider *slider = new QSlider(Qt::Horizontal);
    QPushButton* button = new QPushButton("Quit");

    spinBox->setRange(0, 130);
    slider->setRange(0, 130);
    QObject::connect(spinBox, SIGNAL(valueChanged(int)),
                     slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)),
                     spinBox, SLOT(setValue(int)));
    QObject::connect( button, SIGNAL(clicked()), &app, SLOT(quit()) );
    spinBox->setValue(35);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(spinBox);
    layout->addWidget(slider);
    layout->addWidget(button);
    window->setLayout(layout);
    window->show();

    FindDialog* dialog = new FindDialog;
    dialog->show();

    return app.exec();
}
