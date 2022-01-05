#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTableView>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtWidgets/QHeaderView>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QValueAxis>

#include "customtablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void onClicked();
    void incomingConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();
private:
    Ui::MainWindow *ui;

    const QStringList mainButtonNames = {"Mapa", "Senzsorická data", "Pohled robota", "Debug"};
    QList<QPushButton *> *mainButtonList;

    QChartView *tempGraph;
    QChartView *speedGraph;
    QChartView *distanceGraph;
    QChartView *batteryGraph;

    QList<QChartView *> *graphs;

    const QList<QString> robotColorsHex = {"#8be800", "#ff6ec2", "#74c5ff", "#EDD323"};
    const QStringList robotNames = {"Green", "Pink", "Blue", "Golden"};

    //QList<QChart *> *charts;

    const int graphCount = 2;
    const int ROBOT_COUNT = 4;

    QTcpServer *tcpServer;
    QMap<QString, QTcpSocket *> *tcpSockets;

    QTextBrowser *debugBrowser;

    QScrollArea *scrollArea;

    QList<QList<QList<int> *>> *graphDataLists;
    QStringList graphNames = {"Teplota", "Stav akumulátoru"};
    QStringList graphValueNames = {"Teplota [°C]", "Nabití [%]"};
    QList<int> graphMax = {35, 100};

    QList<QList<int> *> *temperature;
    QList<QList<int> *> *batteryStatus;

    void setMainButtonPosition(QPushButton *button);
    void showMainButtons(bool show);
    void showGraphs(bool show);
    void showDebug(bool show);
    void printDebug(QString data);
    void debugCheckForOverflow();
    QLineSeries *createLineSeries(QList<int> data, int nth, bool toFloat = false);
    void refreshGraphs();
    QChart *createGraphChart(QList<QList<int> *> data, QString name, QString valuesNames, int max = -1, bool toFloat = false);
    void addZeros(QList<QList<int> *> data, int maxData);
};
#endif // MAINWINDOW_H
