#include "simpletextview.h"
#include "simpletextviewer_p.h"
#include <QResizeEvent>
#include <QtGui>

SimpleTextView::SimpleTextView(QWidget* parent)
    : QScrollArea(parent)
{
    textViewer = new SimpleTextViewerPrivate(this);
    setWidget(textViewer);
    setWidgetResizable(true);
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//void SimpleTextView::setData(const QByteArray& data)
//{
//    textViewer->setData(data);
//}

void SimpleTextView::appendData(const QByteArray& data)
{
    textViewer->appendData(data);
    ensureVisible(0, textViewer->height());
}

void SimpleTextView::resizeEvent(QResizeEvent* e)
{
    QScrollArea::resizeEvent(e);
    int w = e->size().width();
    textViewer->setFixedWidth(w);
}
