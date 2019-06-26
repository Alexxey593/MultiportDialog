#ifndef SIMPLETEXTVIEWER_P_H
#define SIMPLETEXTVIEWER_P_H

#include <QScrollArea>
#include <QVector>

class QTextCodec;

class SimpleTextViewerPrivate : public QWidget {
    Q_OBJECT
private:
    QString _dataStr;
    QTextCodec* _codec;
    int _charWidth, _charHeight; // char dimensions (dpendend on font)
    int _charsInLine;
    int _selectionBegin; // First selected char
    int _selectionEnd; // Last selected char
    int _selectionInit; // That's, where we pressed the mouse button
    QScrollArea* _scrollArea;
    QVector<QString> _lines;

public:
    explicit SimpleTextViewerPrivate(QScrollArea* parent);
    void setCodecName(QByteArray name);
//    void setData(const QByteArray& data);
    void appendData(const QByteArray& data);
    virtual void setFont(const QFont& font);

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent*);

signals:

public slots:

private:
    void adjust();
};

#endif // SIMPLETEXTVIEWER_P_H
