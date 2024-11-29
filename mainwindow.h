#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void exportFiles();
    void updateGamesList();
    void browseQmamehookerPath();
    void browseEmulatorPath();
    void browseIniPath();
    void browseDemulPath();
    void browseRomPath();
    void updateEmulatorPath();
    void exportAllFiles();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
