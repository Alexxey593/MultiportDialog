#include "mylistwidget.h"
#include <QMouseEvent>

MyListWidget::MyListWidget(QWidget *parent) :
    QListWidget(parent)
{
}

void MyListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
    if(event->button() == Qt::MiddleButton)
        emit middleButtonClicked();
}
