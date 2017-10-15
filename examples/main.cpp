#include "def.h"
#include "abstractcommand.h"
#include "frontcontroller.h"
#include "view.h"
#include "testcommand.h"
#include <QDebug>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication cApp(argc, argv);

    /// controller, the default one is used (FrontController)
    FrontController* pcFC = FrontController::getInstance();
    pcFC->registerCommand("show_string_command", &FirParamCommand::staticMetaObject);

    /// view
    MainWindow cView;
    cView.show();

    cApp.exec();
    return 0;
}
