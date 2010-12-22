#include <QtGui/QApplication>
#include "maketiny.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MakeTiny w;
    w.createActions();
    w.show();

    return a.exec();
}
