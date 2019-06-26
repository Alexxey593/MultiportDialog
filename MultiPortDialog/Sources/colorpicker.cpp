#include "colorpicker.h"
#include <QColorDialog>

ColorPicker::ColorPicker(QWidget *parent) :
    QLabel(parent)
{
    setColor(Qt::green);
}

void ColorPicker::mousePressEvent(QMouseEvent *)
{
    const QColor color = QColorDialog::getColor(Qt::green, this, tr("Select Color"));
    setColor(color);
}

QColor ColorPicker::color() const
{
    return m_color;
}

void ColorPicker::setColor(const QColor &color)
{
    if (color.isValid()) {
        m_color = color;
        setPalette(QPalette(color));
        setAutoFillBackground(true);
        update();
    }
}
