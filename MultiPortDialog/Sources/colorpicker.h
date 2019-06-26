#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QLabel>
#include <QMouseEvent>

class ColorPicker : public QLabel
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = 0);

    QColor color() const;
    void setColor(const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *);

private:
    QColor m_color;

signals:

public slots:

};

#endif // COLORPICKER_H
