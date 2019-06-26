#include "memcustomplot.h"
#include <QMouseEvent>
#include <QTime>

MemCustomPlot::MemCustomPlot(QWidget *parent) :
    QCustomPlot(parent)
{
    noMove = false;
    xPos = 0.0;
    prevVal = 0.0;
    yRange.upper = 1;
    yRange.lower = 0;
    this->setInteractions(QCP::iRangeDrag);
    this->axisRect()->setRangeDrag(Qt::Horizontal);
    this->axisRect()->setupFullAxesBox();
    this->addGraph();
    this->graph(0)->setPen(QPen(Qt::red));
    this->xAxis->setRange(0, 1);
    this->yAxis->setRange(yRange);
    this->yAxis->setNumberFormat("f");
    this->yAxis->setNumberPrecision(0);
    connect(this, SIGNAL(mousePress(QMouseEvent *)), SLOT(graphPressed(QMouseEvent *)));
    // connect(this, SIGNAL(mouseRelease(QMouseEvent*)), SLOT(graphReleased()));

    textLabel = new QCPItemText(this);
    this->addItem(textLabel);
    textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    // textLabel->setPen(QPen(Qt::black)); // show black border around text

    arrow = new QCPItemLine(this);
    this->addItem(arrow);
    arrow->start->setParentAnchor(textLabel->right);
    arrow->setHead(QCPLineEnding::esSpikeArrow);

    this->addGraph(); // red dot
    this->graph(1)->setPen(QPen(Qt::red));
    this->graph(1)->setLineStyle(QCPGraph::lsNone);
    this->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
}

void MemCustomPlot::graphPressed(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        noMove = true;
    else {
        noMove = false;
        this->xAxis->setRange(xPos, this->axisRect()->width() / 2 - 0.5, Qt::AlignRight);
        replot();
    }
    emit movePaused(noMove);
}

void MemCustomPlot::graphReleased()
{
    noMove = false;
}

void MemCustomPlot::resizeEvent(QResizeEvent *e)
{
    if (!noMove)
        this->xAxis->setRange(xPos, this->axisRect()->width() / 2 - 0.5, Qt::AlignRight);
    QCustomPlot::resizeEvent(e);
}

void MemCustomPlot::closeEvent(QCloseEvent *e)
{
    emit closed();
    e->accept();
}

void MemCustomPlot::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        emit languageChanged();
    else
        QCustomPlot::changeEvent(event);
}

void MemCustomPlot::addPoint(double val)
{
    // val = val / 1024.0 / 1024.0;
    if (val > prevVal)
        this->graph(0)->setPen(QPen(Qt::red));
    else if (val < prevVal)
        this->graph(0)->setPen(QPen(Qt::green));
    else
        this->graph(0)->setPen(QPen(Qt::blue));
    prevVal = val;
    this->graph(0)->addData(xPos, val);
    static double minVal = val;
    static double maxVal = val;
    this->graph(0)->rescaleValueAxis(false, false);
    this->graph(1)->clearData();
    this->graph(1)->addData(xPos, val);
    if (xPos == 0) {
        minVal = val;
        maxVal = val;
        yRange.upper = val + 0.01;
        yRange.lower = val - 0.01;
        textLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
        textLabel->position->setCoords(-20, val);
        arrow->end->setCoords(0, val);
    } else {
        if (val < minVal)
            minVal = val;
        if (val > maxVal)
            maxVal = val;
        if (minVal != maxVal) {
            yRange.upper = maxVal + (maxVal - minVal) * 0.05;
            yRange.lower = minVal - (maxVal - minVal) * 0.05;
            textLabel->position->setCoords(-20, (maxVal - minVal) * 0.5 + minVal);
        }
    }
    this->yAxis->setRange(yRange);
    if (!noMove)
        this->xAxis->setRange(xPos, this->axisRect()->width() / 2 - 0.5, Qt::AlignRight);
    replot();
    xPos++;
} // addPoint

void MemCustomPlot::clear()
{
    xPos = 0.0;
    prevVal = 0.0;
    this->graph(0)->clearData();
    this->graph(1)->clearData();
    yRange.upper = 1;
    yRange.lower = 0;
    this->xAxis->setRange(xPos, this->axisRect()->width() / 2 - 0.5, Qt::AlignRight);
    noMove = false;
    emit movePaused(noMove);
    replot();
} // clear
