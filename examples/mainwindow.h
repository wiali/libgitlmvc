#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "view.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public View
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /// it receives the result from commands
    void onUIUpdate(UpdateUIEvt& rcEvt);
    
private slots:
    void on_mtTestButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
