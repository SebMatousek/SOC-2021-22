#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsColorizeEffect>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

    void setName(QString newName);
    QString getName();

public slots:

signals:
    void label_clicked();
    void label_released();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QString name = "label";
    bool labelPressed = false;

    void greyOut();
};

#endif // CLICKABLELABEL_H
