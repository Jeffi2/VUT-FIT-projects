/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Control program flow, starts application
 *
 */

#include <stdlib.h>

#include <QApplication>
#include "mainwidget.h"

int main(int argc, char * argv[])
{

    QApplication a(argc,argv);
    MainWidget w;
    w.show();

    return a.exec();

}
