#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onClicked(bool clicked);
private:
    Ui::MainWindow *ui;

    const QStringList mainButtonNames = {"Mapa", "Senzsorická data", "Pohled robota", "Debug"};
};
#endif // MAINWINDOW_H
