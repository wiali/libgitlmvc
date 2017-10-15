#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "cmdevt.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    listenToParams("data_to_view", MAKE_CALLBACK(MainWindow::onUIUpdate));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onUIUpdate(UpdateUIEvt &rcEvt)
{

    QString strDataInView = rcEvt.getParameter("data_to_view").toString();
    ui->mtTestLabel->setText(strDataInView);
    qDebug() << strDataInView;

}

void MainWindow::on_mtTestButton_clicked()
{
    /// event (in real case, this event should be dispatch from user interface, i.e. the views)
    CmdEvt cRequestEvt("show_string_command");
    cRequestEvt.setParameter("data_to_command", "Hello MVC");
    cRequestEvt.dispatch();
}
