#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include <QValueAxis>
#include <QScatterSeries>

#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QMouseEvent>

#include "clickablelabel.h"
#include "joystick.h"


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

signals:
    void downloaded();

public slots:
    void keyReleaseEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void onClicked();
    void incomingConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();
    void mapButtonClicked();
    void httpFinished();
    void httpReadyRead();
    void on_joystick_changed(float x, float y);
private:
    Ui::MainWindow *ui;

    QLabel *titleImage;
    const QStringList mainButtonNames = {"Mapa", "Senzory", "Kamera", "Debug"};
    QList<ClickableLabel *> *mainButtonList;

    QList<QChartView *> *graphs;

    const QList<QString> robotColorsHex = {"#8be800", "#ff6ec2", "#74c5ff", "#EDD323"};
    const QStringList robotNames = {"Green", "Pink", "Blue", "Golden"};
    const QStringList czNames = {"Zeleného", "Růžového", "Modrého", "Zlatého"};

    //QList<QChart *> *charts;

    const int graphCount = 4;
    const int ROBOT_COUNT = 4;

    QTcpServer *tcpServer;
    QMap<QString, QTcpSocket *> *tcpSockets;

    QTextBrowser *debugBrowser;

    QList<QList<QList<int> *>> *graphDataLists;
    QStringList graphNames = {"Teplota", "Stav akumulátoru", "Ujetá vzdálenost", "Průměrná rychlost"};
    QStringList graphValueNames = {"Teplota [°C]", "Nabití [%]", "Vzdálenost [cm]", "Rychlost [m/s]"};
    QList<int> graphMax = {35, 100, 0, 0};

    QList<QList<int> *> *temperature;
    QList<QList<int> *> *batteryStatus;
    QList<QList<int> *> *distance;
    QList<QList<int> *> *speed;

    QList<QList<int> *> *sensorDataTime;

    int pageDepth = 0;

    QList<QChartView *> *maps;
    ClickableLabel *mapButton;


    QLabel *cameraLabel;
    Joystick *joystick;
    int lastJ = -1;

    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    std::unique_ptr<QFile> file;

    const QTime startTime = QDateTime::currentDateTime().toLocalTime().time();

    bool mapping = false;
    QList<QList<QList<int> *> *> *mapData;

    QWidget *mainCentralWidget;
    QScrollArea *scrollArea;

    QSize screenSize;

    QList<QWidget *> *mapSpacers;
    QList<QWidget *> *mainSpacers;
    QList<QWidget *> *cameraSpacers;

    QColor white = QColor(240, 240, 240);
    QColor black = QColor(16, 16, 16);
    QColor grey = QColor(25, 25, 25);

    int mapW;

    void setMainButtonPosition(ClickableLabel *button, int nth);
    void showMainButtons(bool show);
    void showGraphs(bool show);
    void showDebug(bool show);
    void debugCheckForOverflow();
    QLineSeries *createLineSeries(QList<int> data, int nth, bool toFloat = false);
    void refreshGraphs();
    QChart *createGraphChart(QList<QList<int> *> data, QString name, QString valuesNames, int max = -1, bool toFloat = false);
    void toMainButtonPage();
    void toGraphPage();
    void toDebugPage();
    QLineSeries *createMapSeries(QList<QList<int> *> *mapData, int nth);
    void showMaps(bool show);
    void toMapPage();

    void showCamera(bool show);
    void toCameraPage();

    void addZeros(QList<QList<int> *> data, int maxDataLength, int maxDataIndex);
    QChart *newMapChart(QList<QList<int> *> *mapData, int nth);
    void refreshMaps(int index);
    void readMessage(QString data, QString *name);
    void printDebug(QString data);
    void scrollToBeginning();
    void downloadFile();
    std::unique_ptr<QFile> openFileForWrite(const QString &fileName);
    void updateCameraLabel();
    void enableJoystick(bool en);
};
#endif // MAINWINDOW_H
