#include "indicator.h"
#include <QPainter>
#include <QDebug>

Indicator::Indicator(QWidget* parent)
    : QWidget(parent)
{
    m_color = this->palette().base().color();
}

QColor Indicator::color() const
{
    return m_color;
}

void Indicator::setColor(const QColor& color)
{
    if (color != m_color) {
        m_color = color;
        update();
    }
}

void Indicator::clearColor()
{
    setColor(this->palette().base().color());
}

QString Indicator::text() const
{
    return m_text;
}

void Indicator::setText(const QString& text)
{
    if (text != m_text) {
        m_text = text;
        update();
    }
}

void Indicator::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(m_color);
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
    QFont fnt = painter.font();
    fnt.setPixelSize(10);
    painter.setFont(fnt);
    painter.drawText(QRectF(0.5, 0.5, this->width() - 0.5, this->height() - 0.5), Qt::AlignCenter, m_text);
}

void Indicator::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    emit clicked();
}
