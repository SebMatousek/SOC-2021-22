#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    labelPressed = true;

    greyOut();
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(labelPressed && this->geometry().contains(event->globalPos()))
    {
        emit label_clicked();
    }
    else
    {
        emit label_released();
    }

    labelPressed = false;

    greyOut();
}

void ClickableLabel::greyOut()
{
    if(labelPressed)
    {
        QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(this);
        effect->setColor(QColor("#8686bd"));
        this->setGraphicsEffect(effect);
    }
    else
    {
        this->setGraphicsEffect(nullptr);
    }
}

void ClickableLabel::setName(QString newName)
{
    name = newName;
}
QString ClickableLabel::getName()
{
    return name;
}
