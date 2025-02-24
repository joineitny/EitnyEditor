#include <QApplication>
#include "EitnyEditor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    EitnyEditor editor;
    editor.show();
    return app.exec();
}
