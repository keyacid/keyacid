#include "widget.h"
#include <QApplication>
#include <sodium.h>

int main(int argc,char **argv) {
    if (sodium_init()==-1) {
        return 1;
    }
    QApplication a(argc,argv);
    Widget w;
    w.show();
    return a.exec();
}
