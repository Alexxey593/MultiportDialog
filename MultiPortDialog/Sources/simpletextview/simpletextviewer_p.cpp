#include "simpletextviewer_p.h"
#include <QTextCodec>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

#define MAXSCROLLHEIGHT 16777215

SimpleTextViewerPrivate::SimpleTextViewerPrivate(QScrollArea* parent)
    : QWidget(parent)
{
    _charWidth = fontMetrics().width(QLatin1Char('W'));
    _charHeight = fontMetrics().height();
    _charsInLine = (width() - 10) / _charWidth;
    _selectionBegin = -1; // First selected char
    _selectionEnd = -1; // Last selected char
    _selectionInit = -1; // That's, where we pressed the mouse button
    _codec = QTextCodec::codecForLocale();
    setFocusPolicy(Qt::StrongFocus);
    setFont(QFont("Terminal", 10));
}

void SimpleTextViewerPrivate::setCodecName(QByteArray name)
{
    _codec = QTextCodec::codecForName(name);
    adjust();
}

//void SimpleTextViewerPrivate::setData(const QByteArray& data)
//{
//    if (_codec)
//        _dataStr = _codec->toUnicode(data);
//    else
//        _dataStr = data;
//    adjust();
//}

void SimpleTextViewerPrivate::appendData(const QByteArray& data)
{
    if (data.isEmpty())
        return;
    QString tmpStr = _codec->toUnicode(data);
    tmpStr.replace("\r\n", "\n");
    tmpStr.replace("\r", "\n");
    _dataStr.append(tmpStr);
    int pos = 0;
    int endPos = 0;
    QString substr;
    while (pos < tmpStr.length()) {
        endPos = tmpStr.indexOf('\n', pos);
        bool wasEol = true;
        if (endPos < 0) {
            endPos = tmpStr.length();
            wasEol = false;
        }
        if (!wasEol)
            substr = tmpStr.mid(pos, endPos - pos);
        else
            substr = tmpStr.mid(pos, endPos - pos + 1);
//        if (!_lines.empty()) {
//            substr.prepend(_lines.last());
//            _lines.removeLast();
//        }
        int lineCnt = substr.length() / _charsInLine + 1;
        for (int i = 0; i < lineCnt; i++) {
            _lines.append(substr.mid(i * _charsInLine, _charsInLine));
        }
//        if (wasEol)
//            _lines.append("");
        pos = endPos + 1;
    }

    //    while (tmpStr.length() > 0) {
    //        int pos = tmpStr.indexOf('\n');
    //        if (pos >= 0) {
    //            QString substr = tmpStr.left(pos);
    //            if (!_lines.empty()) {
    //                substr.prepend(_lines.last());
    //                _lines.removeAt(_lines.size() - 1);
    //            }
    //            tmpStr.remove(0, pos + 1);
    //            while (substr.length() > 0) {
    //                if (substr.length() > _charsInLine) {
    //                    _lines.append(substr.left(_charsInLine));
    //                    substr.remove(0, _charsInLine);
    //                }
    //                else {
    //                    _lines.append(substr);
    //                    break;
    //                }
    //            }
    //            _lines.append("");
    //        }
    //        else {
    //            if (!_lines.empty()) {
    //                tmpStr.prepend(_lines.last());
    //                _lines.removeAt(_lines.size() - 1);
    //            }
    //            while (tmpStr.length() > 0) {
    //                if (tmpStr.length() > _charsInLine) {
    //                    _lines.append(tmpStr.left(_charsInLine));
    //                    tmpStr.remove(0, _charsInLine);
    //                }
    //                else {
    //                    _lines.append(tmpStr);
    //                    break;
    //                }
    //            }
    //        }
    //    }
    int newHeight = _lines.length() * _charHeight + 5;
    if (newHeight < MAXSCROLLHEIGHT)
        setMinimumHeight(newHeight);
    else
        setMinimumHeight(MAXSCROLLHEIGHT - 1);
    //    int eols = tmpStr.count('\n');
    //    int pos = tmpStr.indexOf('\n');
    //    while (pos >= 0) {
    //        QString substr = tmpStr.left(pos);
    //        int len = substr.length() + _charsInLastLine;
    //        if(_charsInLine > 0) {
    //            int newHeight = ((len / _charsInLine + 1) * _charHeight) + 5;
    //        }
    //    }
    update();
}

void SimpleTextViewerPrivate::setFont(const QFont& font)
{
    QWidget::setFont(font);
    adjust();
}

void SimpleTextViewerPrivate::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor("white"));
    if (_charsInLine <= 0 || _lines.isEmpty()) {
        return;
    }
    painter.setPen(this->palette().color(QPalette::WindowText));
    //    int dataHeight = ((_dataStr.size() / _charsInLine + 1) * _charHeight) + 5;
    int dataHeight = (_lines.count() * _charHeight) + 5;
    int firstLineIdx = ((dataHeight - (height() - event->rect().top())) / _charHeight - _charHeight) /* * _charsInLine*/;
    if (firstLineIdx < 0)
        firstLineIdx = 0;
    int lastLineIdx = ((dataHeight - (height() - event->rect().bottom())) / _charHeight + _charHeight) /* * _charsInLine*/;
    //    if (lastLineIdx > _dataStr.size())
    //        lastLineIdx = _dataStr.size();
    if (lastLineIdx >= _lines.count())
        lastLineIdx = _lines.count() - 1;
    int yPosStart = ((firstLineIdx) /* / _charsInLine*/) * _charHeight + _charHeight /* - (dataHeight - minimumHeight())*/;
    for (int lineIdx = firstLineIdx, yPos = yPosStart; lineIdx <= lastLineIdx; lineIdx += /*_charsInLine*/ 1, yPos += _charHeight) {
        int xPos = 5;
        //        for (int colIdx = 0; ((lineIdx + colIdx) < _dataStr.size() && (colIdx < _charsInLine)); colIdx++) {
        QString currStr = _lines.at(lineIdx);
        painter.drawText(xPos, yPos, currStr);
        //        for (int i = 0; i < currStr.length(); i++) {
        //            QChar c = currStr.at(i); //_dataStr.at(lineIdx + colIdx);
        //            if (c == '\n' || c == '\r') {
        //                xPos = 5;
        //                yPos += _charHeight;
        //                continue;
        //            }
        //            painter.drawText(xPos, yPos, /*_dataStr.at(lineIdx + colIdx)*/ c);
        //            xPos += _charWidth;
        //        }
    }
}

void SimpleTextViewerPrivate::resizeEvent(QResizeEvent*)
{
    adjust();
}

void SimpleTextViewerPrivate::adjust()
{
    _charWidth = fontMetrics().width(QLatin1Char('W'));
    _charHeight = fontMetrics().height();
    _charsInLine = (width() - 10) / _charWidth;
    if (_charsInLine <= 0 || _dataStr.isEmpty()) {
        return;
    }
    // tell QAbstractScollbar, how big we are
    QByteArray data = _dataStr.toUtf8();
    _lines.clear();
    _dataStr.clear();
    appendData(data);
    //    int newHeight = (/*(_dataStr.size() / _charsInLine + 1)*/ _lines.length() * _charHeight) + 5;
    //    if (newHeight < MAXSCROLLHEIGHT)
    //        setMinimumHeight(newHeight);
    //    else
    //        setMinimumHeight(MAXSCROLLHEIGHT - 1);
    update();
}
