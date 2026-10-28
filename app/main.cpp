#include <QApplication>
#include "MainWindow.h" // Includem schița ferestrei noastre

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow fereastraPrincipala;

    fereastraPrincipala.show();

    return app.exec();
}