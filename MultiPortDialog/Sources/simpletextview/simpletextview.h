#ifndef SIMPLETEXTVIEW_H
#define SIMPLETEXTVIEW_H

#include <QScrollArea>

class SimpleTextViewerPrivate;
class QTextCodec;

class SimpleTextView : public QScrollArea {
    Q_OBJECT

private:
    SimpleTextViewerPrivate* textViewer;

public:
    explicit SimpleTextView(QWidget* parent = 0);

//    void setData(const QByteArray& data);
    void appendData(const QByteArray& data);

protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // SIMPLETEXTVIEW_H
