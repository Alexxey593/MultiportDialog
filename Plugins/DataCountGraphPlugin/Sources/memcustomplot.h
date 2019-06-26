#ifndef MEMCUSTOMPLOT_H
#define MEMCUSTOMPLOT_H

#include "qcustomplot.h"

class MemCustomPlot : public QCustomPlot {
    Q_OBJECT

public:
    explicit MemCustomPlot(QWidget *parent = 0);

private:
    bool noMove;
    double xPos;
    double prevVal;
    QCPRange yRange;
    QCPItemText *textLabel;
    QCPItemLine *arrow;

private slots:
    void graphPressed(QMouseEvent *e);
    void graphReleased();

protected:
    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *event);

signals:
    void movePaused(bool isPaused);
    void closed();
    void languageChanged();

public slots:
    void addPoint(double val);
    void clear();
};
#endif // MEMCUSTOMPLOT_H
