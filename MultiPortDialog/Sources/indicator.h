#ifndef INDICATOR_H
#define INDICATOR_H

#include <QWidget>

class Indicator : public QWidget {
    Q_OBJECT
public:
    explicit Indicator(QWidget* parent = 0);

    QColor color() const;
    void setColor(const QColor &color);
    void clearColor();

    QString text() const;
    void setText(const QString &text);

private:
    QColor m_color;
    QString m_text;

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);

signals:
    void clicked();

public slots:
};

#endif // INDICATOR_H
