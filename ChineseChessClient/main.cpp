#include "login.h"

#include <QApplication>
#include <checkerboard.h>
#include <maininterface.h>
#include <login.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Login l;

    l.show();

    return a.exec();
}
