#include "mainwindow.h"
#include "ui_mainwindow.h"

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSize screenSize = QGuiApplication::primaryScreen()->size();

    this->setGeometry(0, 0, screenSize.width(), screenSize.height());
    ui->centralwidget->setGeometry(0, 0, screenSize.width(), screenSize.height());

    int mainButtonLength = mainButtonNames.length();
    mainButtonList = new QList<QPushButton *>();

    for(int i = 0; i < mainButtonLength; i++)
    {
        QPushButton *button = new QPushButton(this);
        button->setText(mainButtonNames.at(i));
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
        setMainButtonPosition(button);
        layout()->addWidget(button);
        mainButtonList->append(button);
        button->show();
    }

    temperature = new QList<QList<int> *>();
    batteryStatus = new QList<QList<int> *>();

    for(int i = 0; i < ROBOT_COUNT; i++)
    {
        temperature->append(new QList<int>());
        batteryStatus->append(new QList<int>());
    }

    graphDataLists = new QList<QList<QList<int> *>>();

    graphDataLists->append(*temperature);
    graphDataLists->append(*batteryStatus);


    graphs = new QList<QChartView *>();

    /*QList<QChart *> *charts = new QList<QChart *>();

    for(int i = 0; i < charts->length(); i++)
    {
        charts->append(createGraphChartV2(*data));
    }*/

    tempGraph = new QChartView(createGraphChart(*temperature, graphNames.at(0), graphValueNames.at(0), graphMax.at(0), true));
    batteryGraph = new QChartView(createGraphChart(*batteryStatus, graphNames.at(1), graphValueNames.at(1), graphMax.at(1)));
    //speedGraph = new QChartView(charts->at(1));
    //distanceGraph = new QChartView(charts->at(2));

    graphs->append(tempGraph);
    graphs->append(batteryGraph);
    //graphs->append(speedGraph);
    //graphs->append(distanceGraph);

    int graphWidth = this->width() * 0.95;
    int graphHeight = graphWidth / 1.61;

    for(int i = 0; i < graphCount; i++)
    {
        graphs->at(i)->setRenderHint(QPainter::Antialiasing);
        layout()->addWidget(graphs->at(i));

        graphs->at(i)->setGeometry(0, i * graphWidth + graphWidth / 10, graphWidth, graphHeight);
        graphs->at(i)->hide();
    }

    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, 8090)) {
        QMessageBox::critical(this, "Server",
                              QString("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    qDebug() << QString("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                .arg(ipAddress).arg(tcpServer->serverPort());

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(incomingConnection()));

    tcpSockets = new QMap<QString, QTcpSocket *>();

    debugBrowser = new QTextBrowser(ui->centralwidget);
    debugBrowser->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
    debugBrowser->hide();
}

void MainWindow::debugCheckForOverflow()
{
    QString control = debugBrowser->toPlainText();
    int endls = control.count("\n");
    //qDebug()<<control.count("\n");

    if(endls > 38)
    {
        //qDebug()<<"bigger";

        endls -= 38;

        for(int i = 0; i <= endls; i++)
        {
            //qDebug()<<control.indexOf("\n");
            //qDebug()<<control;
            control.remove(0, control.indexOf("\n") + 1);
        }

        debugBrowser->clear();
        debugBrowser->setText(control);
    }
}

void MainWindow::printDebug(QString data)
{
    while(true)
    {
        if(data.lastIndexOf("\n") == data.length() - 1)
        {
            data.chop(1);
        }
        else break;
    }

    debugBrowser->append(data);

    debugCheckForOverflow();
}

void MainWindow::showDebug(bool show)
{
    if(show)
    {
        debugBrowser->show();
    }
    else
    {
        debugBrowser->hide();
    }
}

void MainWindow::incomingConnection()
{
    QTcpSocket *newSocket = tcpServer->nextPendingConnection();
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(newSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    tcpSockets->insert("Connection" + QString::number(tcpSockets->size()), newSocket);
    printDebug("New connection; IP: " + tcpSockets->last()->peerAddress().toString());
}

void MainWindow::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray d = sender->readAll();

    QString *data = new QString();
    data->append(QString(d));

    QString name = tcpSockets->key(sender);

    qDebug()<<QString(tcpSockets->key(sender)) + " is sending the message: " + data;
    printDebug(QString(tcpSockets->key(sender)) + " wrote: " + data);

    if(d.contains("#"))
    {
        d = d.remove(0, 1);

        qDebug()<<"Before name change";
        qDebug()<<tcpSockets->values();
        qDebug()<<tcpSockets->uniqueKeys();

        QMap<QString, QTcpSocket *> *swapper = new QMap<QString, QTcpSocket *>();

        QList<QString> keys = tcpSockets->uniqueKeys();


        for(int i = 0; i < keys.length(); i++)
        {
            QString compare = QString(keys.at(i));

            if(compare != name)
            {
                swapper->insert(compare, tcpSockets->value(compare));
            }
            else
            {
                swapper->insert(d, tcpSockets->value(compare));
            }

            qDebug()<<compare;
            qDebug()<<name;
        }

        tcpSockets->swap(*swapper);

        qDebug()<<"After name change";
        qDebug()<<tcpSockets->values();
        qDebug()<<tcpSockets->uniqueKeys();
    }
    else
    {
        int senderIndex = robotNames.indexOf(name);

        if(senderIndex == -1)
        {
            qDebug()<<"unknown sender!!!";
            return;
        }
        if(!data->contains("*"))
        {
            qDebug()<<"not a valid format";
            return;
        }

        data->chop(data->length() - data->indexOf("*"));

        qDebug()<<*data;
        qDebug()<<"here ";

        QStringList values = data->split("%");
        qDebug()<<values;

        int temp = values.at(0).toInt();
        int bat = values.at(1).toInt();

        temperature->at(senderIndex)->append(temp);
        batteryStatus->at(senderIndex)->append(bat);

        if(graphs->at(0)->isVisible()) refreshGraphs();
    }
}

void MainWindow::refreshGraphs()
{
    for(int i = 0; i < graphs->length(); i++)
    {
        if(i == 0)
        {
            graphs->at(i)->setChart(createGraphChart(graphDataLists->at(i), graphNames.at(i), graphValueNames.at(i), graphMax.at(i), true));
        }
        else
        {
            graphs->at(i)->setChart(createGraphChart(graphDataLists->at(i), graphNames.at(i), graphValueNames.at(i), graphMax.at(i), false));
        }
    }
}

void MainWindow::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        tcpSockets->remove(tcpSockets->key(sender));
    }
}

QLineSeries * MainWindow::createLineSeries(QList<int> data, int nth, bool toFloat)
{
    QLineSeries *series = new QLineSeries();

    for(int i = 0; i < data.length(); i++)
    {
        if(toFloat)
        {
            series->append(i, data.at(i) / 10);
        }
        else
        {
            series->append(i, data.at(i));
        }
    }


    QPen pen = series->pen();
    pen.setWidth(3);
    pen.setColor(QColor(robotColorsHex.at(nth)));

    series->setPen(pen);

    return series;
}

void MainWindow::addZeros(QList<QList<int> *> data, int maxData)
{
    for(int i = 0; i < data.length(); i++)
    {
        if(data.at(i)->length() < maxData - 3)
        {
            data.at(i)->append(0);
        }
    }
}

QChart * MainWindow::createGraphChart(QList<QList<int> *> data, QString name, QString valuesNames, int max, bool toFloat)
{
    QChart *chart = new QChart();
    chart->legend()->hide();

    for(int i = 0; i < data.length(); i++)
    {
        chart->addSeries(createLineSeries(*data.at(i), i, toFloat));
    }

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Čas [t]");
    axisX->setLabelFormat("%i");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(valuesNames);
    axisY->setLabelFormat("%i");

    int maxData = max;

    if(max < 1)
    {
        for(int i = 0; i < data.length(); i++)
        {
            for(int j = 0; j < data.at(i)->length(); j++)
            {
                int d = data.at(i)->at(j);
                if(maxData < d)
                {
                    maxData = d;
                }
            }
        }
    }

    int maxDataLength = 0;

    for(int i = 0; i < data.length(); i++)
    {
        int d = data.at(i)->length();
        if(maxDataLength < d)
        {
            maxDataLength = d;
        }
    }

    addZeros(data, maxDataLength);

    for(int i = 0; i < data.length(); i++)
    {
        if(max > 1)
        {
            axisY->setRange(0, maxData);
        }
        else
        {
            axisY->setRange(0, maxData + maxData / 50);
        }

        axisX->setRange(0, maxDataLength -1);
    }

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i < data.length(); i++)
    {
        chart->series().at(i)->attachAxis(axisX);
        chart->series().at(i)->attachAxis(axisY);
    }

    chart->setTitle(name);

    return chart;
}

void MainWindow::setMainButtonPosition(QPushButton *button)
{
    button->setGeometry(button->x(), button->y(),
                        ui->centralwidget->width() * 0.85,
                        ui->centralwidget->height() / 9);

    button->setGeometry((ui->centralwidget->width() / 2) - button->width() / 2,
                        mainButtonNames.indexOf(button->text()) * (this->height() / 5) + (this->height() / 7),
                        button->width(), button->height());
}

void MainWindow::showMainButtons(bool show)
{
    if(show)
    {
        for(int i = 0; i < mainButtonList->length(); i++)
        {
            mainButtonList->at(i)->show();
        }
    }
    else
    {
        for(int i = 0; i < mainButtonList->length(); i++)
        {
            mainButtonList->at(i)->hide();
        }
    }
}

void MainWindow::showGraphs(bool show)
{
    if(show)
    {
        for(int i = 0; i < graphCount; i++)
        {
            graphs->at(i)->show();
            //ui->scrollArea->show();
        }
    }
    else
    {
        for(int i = 0; i < graphCount; i++)
        {
            graphs->at(i)->hide();
            //ui->scrollArea->hide();
        }
    }
}

void MainWindow::onClicked()
{
    auto sender = (QPushButton *)this->sender();
    //qDebug()<<mainButtonNames.indexOf(sender->text());

    int buttonPressed = mainButtonNames.indexOf(sender->text());
    /*
    for(int i = 0; i < graphCount; i++)
    {
        graphModels->at(i)->addData(20, graphModels->at(i)->index(graphModels->at(i)->rowCount() -1, graphModels->at(i)->columnCount() -1));
        graphs->at(i)->repaint();
    }
*/

    //graphs->at(0)->chart()->series().at(0)->

    switch(buttonPressed)
    {
    case 0:
        //qDebug()<<"mapa";
        break;
    case 1:
        //qDebug()<<"data";
        showMainButtons(false);
        refreshGraphs();
        showGraphs(true);

        break;
    case 2:
        //qDebug()<<"kamera";
        break;
    case 3:
        //qDebug()<<"debug";
        showMainButtons(false);
        showDebug(true);
        break;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
