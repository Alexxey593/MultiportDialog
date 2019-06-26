#include "utmscannerwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtMath>
#include <QWheelEvent>
#include <QMouseEvent>

UTMScannerWidget::UTMScannerWidget(QWidget* parent)
    :
#ifdef USE_GL
    QOpenGLWidget(parent)
#else
    QWidget(parent)
#endif
    , scale(1.0)
    , mousePressed(false)
{
//#ifdef USE_GL
//    this->setFormat(QSurfaceFormat()); //    QOpenGLFormat(QGL::SampleBuffers /*| QGL::HasOverlay*/
//#endif
}

void UTMScannerWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), QColor("white"));
    painter.translate(this->width() / 2 + delta.x(), this->height() / 2 + delta.y());
    painter.scale(scale, scale);
    float halfWidth = (this->width() / 2) / scale;
    float halfHeight = (this->height() / 2) / scale;
    // Net
    QPen pen("gray");
    pen.setWidth(1.0 / scale);
    painter.setPen(pen);
    for (int xPos = 100; xPos * scale <= this->width() / 2 - delta.x(); xPos += 100) {
        if ((xPos % 1000) == 0) {
            pen.setWidth(3.0 / scale);
            painter.setPen(pen);
            painter.drawLine(QPointF(xPos, -halfHeight - delta.y() / scale), QPointF(xPos, halfHeight - delta.y() / scale));
            pen.setWidth(1.0 / scale);
            painter.setPen(pen);
        }
        else
            painter.drawLine(QPointF(xPos, -halfHeight - delta.y() / scale), QPointF(xPos, halfHeight - delta.y() / scale));
    }
    for (int xPos = -100; xPos * scale >= -this->width() / 2 - delta.x(); xPos -= 100) {
        if ((xPos % 1000) == 0) {
            pen.setWidth(3.0 / scale);
            painter.setPen(pen);
            painter.drawLine(QPointF(xPos, -halfHeight - delta.y() / scale), QPointF(xPos, halfHeight - delta.y() / scale));
            pen.setWidth(1.0 / scale);
            painter.setPen(pen);
        }
        else
            painter.drawLine(QPointF(xPos, -halfHeight - delta.y() / scale), QPointF(xPos, halfHeight - delta.y() / scale));
    }
    for (int yPos = 100; yPos * scale <= this->height() / 2 - delta.y(); yPos += 100) {
        if ((yPos % 1000) == 0) {
            pen.setWidth(3.0 / scale);
            painter.setPen(pen);
            painter.drawLine(QPointF(-halfWidth - delta.x() / scale, yPos), QPointF(halfWidth - delta.x() / scale, yPos));
            pen.setWidth(1.0 / scale);
            painter.setPen(pen);
        }
        else
            painter.drawLine(QPointF(-halfWidth - delta.x() / scale, yPos), QPointF(halfWidth - delta.x() / scale, yPos));
    }
    for (int yPos = -100; yPos * scale >= -this->height() / 2 - delta.y(); yPos -= 100) {
        if ((yPos % 1000) == 0) {
            pen.setWidth(3.0 / scale);
            painter.setPen(pen);
            painter.drawLine(QPointF(-halfWidth - delta.x() / scale, yPos), QPointF(halfWidth - delta.x() / scale, yPos));
            pen.setWidth(1.0 / scale);
            painter.setPen(pen);
        }
        else
            painter.drawLine(QPointF(-halfWidth - delta.x() / scale, yPos), QPointF(halfWidth - delta.x() / scale, yPos));
    }
    // Axes
    pen.setWidth(2.0 / scale);
    pen.setColor("blue");
    painter.setPen(pen);
    painter.drawLine(QPointF(-halfWidth - delta.x() / scale, 0), QPointF(halfWidth - delta.x() / scale, 0));
    painter.drawLine(QPointF(0, -halfHeight - delta.y() / scale), QPointF(0, halfHeight - delta.y() / scale));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(-6000, -6000, 12000, 12000);
    pen.setWidth(1);
    pen.setColor("red");
    painter.setPen(pen);
    painter.setBrush(QBrush("red"));
    painter.rotate(135);
    for (int i = 0; i < distances.size(); i++) {
        pen.setWidth(1.0 / scale);
        pen.setColor("green");
        painter.setPen(pen);
        painter.drawLine(QPoint(0, 0), QPoint(0, -distances.at(i) / 10));
        pen.setColor("red");
        pen.setWidth(2.0 / scale);
        painter.setPen(pen);
        painter.drawPoint(0, -distances.at(i) / 10);
        painter.rotate(-0.25);
    }
}

void UTMScannerWidget::wheelEvent(QWheelEvent* event)
{
    scale *= pow((double)2, event->delta() / 240.0);
    if (scale < 0.05)
        scale = 0.05;
    if (scale > 5)
        scale = 5;
    update();
}

void UTMScannerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mousePressed) {
        delta += (event->pos() - prevPos);
        prevPos = event->pos();
        update();
    }
}

void UTMScannerWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        prevPos = event->pos();
        mousePressed = true;
    }
}

void UTMScannerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        mousePressed = false;
    }
}

void UTMScannerWidget::setDistances(QList<qint32> newDistances)
{
    distances = newDistances;
    update();
}
