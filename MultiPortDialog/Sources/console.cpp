/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <scapig@yandex.ru>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "console.h"

#include <QScrollBar>
#include <QTextBlock>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent), sendByLine(false)
{
    document()->setMaximumBlockCount(100);
    enterValue = QByteArray("\x0d", 1);
}

void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setSendByLine(bool value)
{
    sendByLine = value;
    buf.clear();
}

void Console::setEnterValue(const QByteArray &value)
{
    enterValue = value;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
      case Qt::Key_Delete:
      case Qt::Key_Left:
      case Qt::Key_Right:
      case Qt::Key_Up:
      case Qt::Key_Down:
      case Qt::Key_Home:
      case Qt::Key_End:
      case Qt::Key_PageUp:
      case Qt::Key_PageDown:
          // skip processing
          break;

      case Qt::Key_Enter:
      case Qt::Key_Return:
          QPlainTextEdit::keyPressEvent(e);
          if (sendByLine) {
              buf.append(enterValue);
              emit getData(buf);
              buf.clear();
          } else
              emit getData(enterValue);
          break;

      case Qt::Key_Backspace:
          if (sendByLine) {
              if (document()->lastBlock().text().size() > 0) {
                  QPlainTextEdit::keyPressEvent(e);
                  buf.chop(1);
              }
          }
          break;

      default:
          QPlainTextEdit::keyPressEvent(e);
          if (sendByLine)
              buf.append(e->text().toLocal8Bit());
          else
              emit getData(e->text().toLocal8Bit());
    } // switch
}     // keyPressEvent

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
// QPlainTextEdit::mousePressEvent(e);
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
// QPlainTextEdit::contextMenuEvent(e);
}
