#ifndef UTMSCANNERWIDGET_H
#define UTMSCANNERWIDGET_H

#define USE_GL

#ifdef USE_GL
#include <QOpenGLWidget>
class UTMScannerWidget : public QOpenGLWidget
#else
#include <QWidget>
class UTMScannerWidget : public QWidget
#endif
{
    Q_OBJECT
private:
    QList<qint32> distances;
    qreal scale;
    QPoint prevPos;
    QPoint delta;
    bool mousePressed;

public:
    explicit UTMScannerWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
//    void resizeEvent(QResizeEvent *);
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:
    void setDistances(QList<qint32> newDistances);

};

#endif // UTMSCANNERWIDGET_H
