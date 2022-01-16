#include "mainwindow.h"
#include "ui_mainwindow.h"

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    screenSize = QGuiApplication::primaryScreen()->size();
    ui->centralwidget->resize(screenSize.width(), screenSize.height());
    this->resize(screenSize.width(), screenSize.height());

    scrollArea = new QScrollArea(this);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollArea->horizontalScrollBar()->setEnabled(false);


    QWidget * pWgt = new QWidget;
    pWgt->resize(screenSize.width(), screenSize.height());

    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->setSizeConstraint(QLayout::SetMinimumSize);

    pLayout->setContentsMargins(0, 0, 0, 0);

    //Title image

    mainSpacers = new QList<QWidget *>();

    QWidget *w = new QWidget();
    w->setFixedSize(50, 100);

    mainSpacers->append(w);
    pLayout->addWidget(mainSpacers->at(0));

    titleImage = new QLabel();

    int titleImageW = ui->centralwidget->width() * 0.95;
    int titleImageH = ui->centralwidget->height() * 0.25;

    titleImage->setGeometry(ui->centralwidget->width() / 2 - titleImageW / 2, ui->centralwidget->height() * 0.035,titleImageW, titleImageH);
    titleImage->setPixmap(QPixmap(":/img/TitleImage.png").scaledToWidth(titleImageW, Qt::SmoothTransformation));

    pLayout->addWidget(titleImage);
    pLayout->setAlignment(titleImage, Qt::AlignCenter);
    titleImage->show();

    mainSpacers->append(w);
    pLayout->addWidget(mainSpacers->at(1));

    //Main buttons
    int mainButtonLength = mainButtonNames.length();
    mainButtonList = new QList<ClickableLabel *>();

    for(int i = 0; i < mainButtonLength; i++)
    {
        ClickableLabel *button = new ClickableLabel(this);
        connect(button, SIGNAL(label_clicked()), this, SLOT(onClicked()));
        setMainButtonPosition(button, i);
        button->setName(mainButtonNames.at(i));
        mainButtonList->append(button);
        pLayout->addWidget(button);
        pLayout->setAlignment(button, Qt::AlignCenter);
        button->show();
    }

    w = new QWidget();
    w->setFixedSize(50, 200);
    mainSpacers->append(w);
    pLayout->addWidget(mainSpacers->at(2));

    //Graphs
    temperature = new QList<QList<int> *>();
    batteryStatus = new QList<QList<int> *>();
    distance = new QList<QList<int> *>();
    speed = new QList<QList<int> *>();

    for(int i = 0; i < ROBOT_COUNT; i++)
    {
        temperature->append(new QList<int>());
        batteryStatus->append(new QList<int>());
        distance->append(new QList<int>());
        speed->append(new QList<int>());
    }

    graphDataLists = new QList<QList<QList<int> *>>();

    sensorDataTime = new QList<QList<int> *>();
    sensorDataTime->append(new QList<int>());
    sensorDataTime->append(new QList<int>());
    sensorDataTime->append(new QList<int>());
    sensorDataTime->append(new QList<int>());

    graphDataLists->append(*temperature);
    graphDataLists->append(*batteryStatus);
    graphDataLists->append(*distance);
    graphDataLists->append(*speed);


    graphs = new QList<QChartView *>();

    graphs->append(new QChartView(createGraphChart(*temperature, graphNames.at(0), graphValueNames.at(0), graphMax.at(0), true)));
    graphs->append(new QChartView(createGraphChart(*batteryStatus, graphNames.at(1), graphValueNames.at(1), graphMax.at(1))));
    graphs->append(new QChartView(createGraphChart(*distance, graphNames.at(2), graphValueNames.at(2), graphMax.at(2))));
    graphs->append(new QChartView(createGraphChart(*speed, graphNames.at(3), graphValueNames.at(3), graphMax.at(3))));

    int graphWidth = this->width() * 0.95;
    int graphHeight = graphWidth / 1.61;

    for(int i = 0; i < graphCount; i++)
    {
        graphs->at(i)->setFixedSize(graphWidth, graphHeight);

        graphs->at(i)->setRenderHint(QPainter::Antialiasing);
        pLayout->addWidget(graphs->at(i));

        pLayout->setAlignment(graphs->at(i), Qt::AlignCenter);

        graphs->at(i)->hide();
    }

    //WiFi communication
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

    //Debug textBrowser
    debugBrowser = new QTextBrowser(this);
    debugBrowser->setFixedSize(screenSize.width(), screenSize.height());

    pLayout->addWidget(debugBrowser);
    pLayout->setAlignment(debugBrowser, Qt::AlignCenter);

    debugBrowser->hide();

    //Map
    maps = new QList<QChartView *>();
    mapData = new QList<QList<QList<int> *> *>();
    QList<QList<int> *> *startMapData = new QList<QList<int> *>();

    startMapData->append(new QList<int>({0, 0}));

    for(int i = 0; i < ROBOT_COUNT; i++)
    {
        mapData->append(startMapData);
        maps->append(new QChartView(newMapChart(mapData->at(i), i), this));
    }

    //Map Button
    mapButton = new ClickableLabel(this);
    mapButton->setName("MapButton");

    connect(mapButton, SIGNAL(label_clicked()), this, SLOT(mapButtonClicked()));

    int mapBWidth = screenSize.width() * 0.7;
    int mapBHeight = screenSize.height() * 0.08;

    mapButton->setMinimumSize(mapBWidth, mapBHeight);
    mapButton->setMaximumWidth(mapBWidth);
    mapButton->setPixmap(QPixmap(":/img/BeginMapping.png").scaledToWidth(mapBWidth, Qt::SmoothTransformation));

    mapSpacers = new QList<QWidget *>();

    w = new QWidget();
    w->setFixedSize(50, 75);

    mapSpacers->append(w);
    mapSpacers->append(w);

    pLayout->addWidget(mapSpacers->at(0));
    pLayout->addWidget(mapButton);
    pLayout->setAlignment(mapButton, Qt::AlignCenter);

    pLayout->addWidget(mapSpacers->at(1));


    mapButton->hide();
    //-------------

    for(int i = 0; i < maps->length(); i++)
    {
        maps->at(i)->setRenderHint(QPainter::Antialiasing);
        maps->at(i)->setMinimumSize(graphWidth, graphWidth);

        pLayout->addWidget(maps->at(i));
        pLayout->setAlignment(maps->at(i), Qt::AlignCenter);

        mapSpacers->append(w);
        pLayout->addWidget(mapSpacers->at(i+2));

        maps->at(i)->hide();
    }

    //Camera
    cameraLabel = new QLabel();

    cameraLabel->setFixedSize(graphWidth, graphHeight);
    cameraLabel->setPixmap(QPixmap(":/img/loadingImage").scaledToWidth(cameraLabel->width(), Qt::SmoothTransformation));

    pLayout->addWidget(cameraLabel);
    pLayout->setAlignment(cameraLabel, Qt::AlignCenter);
    cameraLabel->hide();

    joystick = new Joystick(this, screenSize.width() * 0.95, screenSize.width() * 0.95);
    pLayout->addWidget(joystick);
    pLayout->setAlignment(joystick, Qt::AlignCenter);
    joystick->hide();
    joystick->setX(0.5);
    joystick->setY(0.5);

    connect(joystick, &Joystick::xChanged, this, [this](float x){
        on_joystick_changed(x, joystick->y());
    });


    connect(joystick, &Joystick::yChanged, this, [this](float y){
        on_joystick_changed(joystick->x(), y);
    });

    cameraSpacers = new QList<QWidget *>();

    w = new QWidget();
    w->setFixedSize(50, 200);

    cameraSpacers->append(w);
    pLayout->addWidget(cameraSpacers->at(0));

    /*
    for(int i = 0; i < pLayout->count(); i++)
    {
        pLayout->itemAt(i)->widget()->setStyleSheet("border: 5px solid red;");
    }*/

    pWgt->setLayout(pLayout);

    //This sentence setwidget must be put in pwgt after all the contents are prepared, otherwise there is a problem with the display
    scrollArea->setWidget(pWgt);
    setCentralWidget(scrollArea);

    toMainButtonPage();
}

void MainWindow::on_joystick_changed(float x, float y)
{
    qDebug()<< x << y;
}

void MainWindow::downloadFile()
{
    //qDebug()<<"Trying to download";
    file = openFileForWrite(QDir::currentPath() + "/ArduCamImage.jpg");

    reply = qnam.get(QNetworkRequest(QUrl::fromUserInput("10.0.0.32/capture")));
    connect(reply, &QNetworkReply::finished, this, &MainWindow::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &MainWindow::httpReadyRead);
}

std::unique_ptr<QFile> MainWindow::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
        return nullptr;
    }
    return file;
}

void MainWindow::httpFinished()
{
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }

    reply->deleteLater();
    reply = nullptr;

    updateCameraLabel();
}

void MainWindow::httpReadyRead()
{
    if (file)
        file->write(reply->readAll());
}

void MainWindow::updateCameraLabel()
{
    //qDebug()<<"pixmap loaded";

    if(cameraLabel->isVisible())
    {
        QPixmap pix = QPixmap(QDir::currentPath() + "/ArduCamImage.jpg");
        if(!pix.isNull())
        {
            cameraLabel->setPixmap(pix.scaledToWidth(cameraLabel->width(), Qt::SmoothTransformation));
        }
        else
        {
            cameraLabel->setPixmap(QPixmap(":/img/noImage").scaledToWidth(cameraLabel->width(), Qt::SmoothTransformation));
        }

        cameraLabel->repaint();
        downloadFile();
    }
}

void MainWindow::mapButtonClicked()
{
    mapping = !mapping;

    if(mapping)
    {
        mapButton->setPixmap(QPixmap(":/img/PauseMapping.png").scaledToWidth(mapButton->width(), Qt::SmoothTransformation));

        QList<QString> keys = tcpSockets->keys();

        for(int i = 0; i < tcpSockets->size(); i++)
        {
            tcpSockets->value(keys.at(i))->write("go");
        }
    }
    else
    {
        mapButton->setPixmap(QPixmap(":/img/BeginMapping.png").scaledToWidth(mapButton->width(), Qt::SmoothTransformation));

        QList<QString> keys = tcpSockets->keys();

        for(int i = 0; i < tcpSockets->size(); i++)
        {
            tcpSockets->value(keys.at(i))->write("stop");
        }
    }
}

QLineSeries * MainWindow::createMapSeries(QList<QList<int> *> *mapData, int nth)
{
    QLineSeries *series = new QLineSeries();

    for(int i = 0; i < mapData->length(); i++)
    {
        series->append(mapData->at(i)->at(0) / 100, mapData->at(i)->at(1) / 100);
    }

    QPen pen = series->pen();
    pen.setWidth(3);
    pen.setColor(QColor(robotColorsHex.at(nth)));

    series->setPen(pen);

    return series;
}

QChart * MainWindow::newMapChart(QList<QList<int> *> *mapData, int nth)
{
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(createMapSeries(mapData, nth));

    chart->setTitle("Mapa Robota " + QString::number(nth + 1));

    int maxData = 0;

    for(int i = 0; i < mapData->length(); i++)
    {
        int x = mapData->at(i)->at(0) / 100;
        int y = mapData->at(i)->at(1) / 100;

        if(maxData < abs(x))
        {
            maxData = abs(x);
        }
        if(maxData < abs(y))
        {
            maxData = abs(y);
        }
    }

    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    axisX->setMinorTickCount(2);
    axisY->setMinorTickCount(2);

    if(maxData != 0)
    {
        axisX->setRange(-maxData - maxData / 10, maxData + maxData / 10);
        axisY->setRange(-maxData - maxData / 10, maxData + maxData / 10);
    }
    else
    {
        axisX->setRange(-10, 10);
        axisY->setRange(-10, 10);
    }

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->series().at(0)->attachAxis(axisX);
    chart->series().at(0)->attachAxis(axisY);

    chart->legend()->hide();

    return chart;
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
    printDebug(*new QString("New connection; IP: " + tcpSockets->last()->peerAddress().toString()));
}

void MainWindow::readMessage(QString data, QString *name)
{
    if(data != "")
    {
        printDebug(*name + " wrote: " + data);
        if(data.contains("@"))
        {
            //comment message
        }
        else if(data.contains("#"))
        {
            qDebug()<<"name change";

            data = data.remove(0, 1);

            tcpSockets->remove(data);

            qDebug()<<"Before name change";
            qDebug()<<tcpSockets->values();
            qDebug()<<tcpSockets->keys();

            QMap<QString, QTcpSocket *> *swapper = new QMap<QString, QTcpSocket *>();

            QList<QString> keys = tcpSockets->keys();


            for(int i = 0; i < keys.length(); i++)
            {
                QString compare = QString(keys.at(i));

                if(compare != name)
                {
                    swapper->insert(compare, tcpSockets->value(compare));
                }
                else
                {
                    swapper->insert(data, tcpSockets->value(compare));
                }

                qDebug()<<compare;
                qDebug()<<name;
            }

            tcpSockets->swap(*swapper);

            qDebug()<<"After name change";
            qDebug()<<tcpSockets->values();
            qDebug()<<tcpSockets->keys();

            if(data == "Golden")
            {
                tcpSockets->value("Golden")->write("getPic");
            }
        }
        else if(data.contains("M"))
        {
            qDebug()<<"map";

            int senderIndex = robotNames.indexOf(*name);

            if(senderIndex == -1)
            {
                qDebug()<<"unknown sender!!!";
                return;
            }

            data.remove(0, 2);

            QStringList values = data.split("%");

            qDebug()<<values;

            int y = 0;
            int x = 0;

            switch(values.at(0).toInt())
            {
            case 0://rotation 0 degrees
                y = values.at(1).toInt();
                break;

            case 90://rotated right
                x = values.at(1).toInt();
                break;

            case -90://rotated left
                x = -(values.at(1).toInt());
                break;

            case 180://rotated left
                y = -(values.at(1).toInt());
                break;

            case -180://rotated left
                y = -(values.at(1).toInt());
                break;

            default:
                qDebug()<<"rotation unrecognized!";
            }

            int mapIndex = robotNames.indexOf(*name);

            x = mapData->at(mapIndex)->at(mapData->at(mapIndex)->length() - 1)->at(0) + x;
            y = mapData->at(mapIndex)->at(mapData->at(mapIndex)->length() - 1)->at(1) + y;

            mapData->at(mapIndex)->append(new QList<int>({x, y}));

            refreshMaps(mapIndex);
            qDebug()<<QString::number(mapIndex);

        }
        else if(data.contains("%"))
        {
            qDebug()<<"sensor data";

            int senderIndex = robotNames.indexOf(*name);

            if(senderIndex == -1)
            {
                qDebug()<<"unknown sender!!!";
                return;
            }

            //qDebug()<<data;
            //qDebug()<<"here ";

            QStringList values = data.split("%");
            qDebug()<<"haha" << values;

            int temp = values.at(0).toInt();
            int bat = values.at(1).toInt();


            sensorDataTime->at(senderIndex)->append(startTime.secsTo(QDateTime::currentDateTime().toLocalTime().time()));

            //qDebug()<<startTime.toString();
            //qDebug()<<QString::number(startTime.secsTo(QDateTime::currentDateTime().toLocalTime().time()));

            temperature->at(senderIndex)->append(temp);
            batteryStatus->at(senderIndex)->append(bat);

            if(graphs->at(0)->isVisible()) refreshGraphs();
        }
    }
}

void MainWindow::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray d = sender->readAll();

    QString *data = new QString();
    data->append(QString(d));

    //qDebug()<<*data;

    QStringList *dataList = new QStringList(data->split("*"));
    QString *name = new QString();
    name->append(tcpSockets->key(sender));

    //qDebug()<<"b" + dataList->at(0);


    for(int i = 0; i < dataList->length(); i++)
    {
        readMessage(dataList->at(i), name);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Back)
    {
        //qDebug("back button pressed");
        if(pageDepth == 1)
        {
            toMainButtonPage();
        }
    }
}

void MainWindow::scrollToBeginning()
{
    scrollArea->scroll(0, 0);
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

void MainWindow::refreshMaps(int index)
{
    maps->at(index)->setChart(newMapChart(mapData->at(index), index));
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
            series->append(sensorDataTime->at(nth)->at(i), data.at(i) / 10);
        }
        else
        {
            series->append(sensorDataTime->at(nth)->at(i), data.at(i));
        }
    }


    QPen pen = series->pen();
    pen.setWidth(3);
    pen.setColor(QColor(robotColorsHex.at(nth)));

    series->setPen(pen);

    return series;
}

void MainWindow::addZeros(QList<QList<int> *> data, int maxDataLength, int maxDataIndex)
{
    for(int i = 0; i < data.length(); i++)
    {
        if(data.at(i)->length() < maxDataLength - 7)
        {
            data.at(i)->append(0);

            for(int j = sensorDataTime->at(i)->length(); j < maxDataLength - 7; j++)
            {
                //qDebug()<<"should append time " + QString::number(sensorDataTime->at(maxDataIndex)->at(j));
                sensorDataTime->at(i)->append(sensorDataTime->at(maxDataIndex)->at(j));
            }
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
    axisX->setTitleText("Čas [s]");
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

    int maxDataIndex = 0;
    int maxDataLength = 0;

    for(int i = 0; i < data.length(); i++)
    {
        int d = data.at(i)->length();
        if(maxDataLength < d)
        {
            maxDataLength = d;
            maxDataIndex = i;
        }
    }

    addZeros(data, maxDataLength, maxDataIndex);

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

        if(maxDataIndex > 0)
        {
            axisX->setRange(0, sensorDataTime->at(maxDataIndex)->at(sensorDataTime->at(maxDataIndex)->length() - 1));
        }
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

void MainWindow::setMainButtonPosition(ClickableLabel *button, int nth)
{
    button->setGeometry(button->x(), button->y(), ui->centralwidget->width() * 0.75, ui->centralwidget->height() / 10);

    button->setPixmap(QPixmap(":/img/" + mainButtonNames.at(nth) + ".png").scaled(button->width(), button->height(), Qt::KeepAspectRatio));


    button->setGeometry((ui->centralwidget->width() / 2) - button->width() / 2,
                        nth * (this->height() / 1.75) + (this->height() / 5) + titleImage->height(),
                        button->width(), button->height());
}

void MainWindow::toMainButtonPage()
{
    pageDepth = 0;

    scrollArea->widget()->setFixedSize(screenSize.width(), screenSize.height());

    showMainButtons(true);

    showCamera(false);

    showGraphs(false);
    showDebug(false);

    showMaps(false);

    scrollToBeginning();

    QScroller::ungrabGesture(scrollArea);
}

void MainWindow::toGraphPage()
{
    pageDepth = 1;

    scrollArea->widget()->setFixedSize(screenSize.width(), screenSize.height() * 2);

    showMainButtons(false);

    refreshGraphs();
    showGraphs(true);

    showCamera(false);
    showDebug(false);

    showMaps(false);

    scrollToBeginning();

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);
}

void MainWindow::toDebugPage()
{
    pageDepth = 1;

    showMainButtons(false);

    showGraphs(false);
    showMaps(false);
    showCamera(false);

    showDebug(true);

    scrollToBeginning();

    QScroller::ungrabGesture(scrollArea);
}

void MainWindow::toMapPage()
{
    pageDepth = 1;

    scrollArea->widget()->setFixedSize(screenSize.width(), screenSize.height() * 2);

    showMainButtons(false);

    showGraphs(false);
    showDebug(false);
    showCamera(false);

    showMaps(true);

    scrollToBeginning();

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);
}

void MainWindow::toCameraPage()
{
    pageDepth = 1;

    showMainButtons(false);

    showGraphs(false);
    showDebug(false);

    showCamera(true);

    showMaps(false);

    scrollToBeginning();

    downloadFile();

    QScroller::ungrabGesture(scrollArea);
}

void MainWindow::showCamera(bool show)
{
    if(show)
    {
        cameraLabel->show();
        joystick->show();

        for(QWidget *w : *cameraSpacers)
        {
            w->show();
        }
    }
    else
    {
        cameraLabel->hide();
        joystick->hide();

        for(QWidget *w : *cameraSpacers)
        {
            w->hide();
        }
    }
}

void MainWindow::showMainButtons(bool show)
{
    if(show)
    {
        for(int i = 0; i < mainButtonList->length(); i++)
        {
            mainButtonList->at(i)->show();
        }
        titleImage->show();

        for(QWidget *w : *mainSpacers)
        {
            w->show();
        }
    }
    else
    {
        for(int i = 0; i < mainButtonList->length(); i++)
        {
            mainButtonList->at(i)->hide();
        }
        titleImage->hide();

        for(QWidget *w : *mainSpacers)
        {
            w->hide();
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

void MainWindow::showMaps(bool show)
{
    if(show)
    {
        for(int i = 0; i < maps->length(); i++)
        {
            maps->at(i)->show();
        }
        mapButton->show();

        for(QWidget *w : *mapSpacers)
        {
            w->show();
        }

    }
    else
    {
        for(int i = 0; i < maps->length(); i++)
        {
            maps->at(i)->hide();
        }
        mapButton->hide();

        for(QWidget *w : *mapSpacers)
        {
            w->hide();
        }
    }
}

void MainWindow::onClicked()
{
    auto sender = (ClickableLabel *)this->sender();
    //qDebug()<<mainButtonNames.indexOf(sender->text());

    int buttonPressed = mainButtonNames.indexOf(sender->getName());
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
        toMapPage();
        break;
    case 1:
        //qDebug()<<"data";
        toGraphPage();

        break;
    case 2:
        //qDebug()<<"kamera";
        toCameraPage();
        break;
    case 3:
        //qDebug()<<"debug";
        toDebugPage();
        break;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
