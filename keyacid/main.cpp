#include "widget.h"
#include "disclaimerdialog.h"
#include <QApplication>
#include <sodium.h>

int main(int argc,char **argv) {
    if (sodium_init()==-1) {
        return 1;
    }
    QApplication a(argc,argv);
    Widget w;
    w.show();
    DisclaimerDialog d;
    d.show();
    return a.exec();
}
