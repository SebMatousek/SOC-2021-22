#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int mainButtonLength = mainButtonNames.length();

    for(int i = 0; i < mainButtonLength; i++)
    {
        QPushButton *button = new QPushButton(this);
        button->setText(mainButtonNames.at(i));
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
        layout()->addWidget(button);
        button->show();
    }
}

void MainWindow::onClicked(bool clicked)
{
    qDebug()<<"clicked";
}

MainWindow::~MainWindow()
{
    delete ui;
}

