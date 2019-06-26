#include "hexvalidator.h"

HexValidator::HexValidator(QObject * parent) : QValidator(parent)
{}

void HexValidator::fixup(QString &input) const
{
    QString temp;
    int index = 0;
    // every 2 digits insert a space if they didn't explicitly type one
    Q_FOREACH(QChar ch, input) {
        if (isxdigit(ch.toLatin1())) {
            if ((index != 0) && ((index & 1) == 0))
                temp += ' ';

            temp += ch.toUpper();
            ++index;
        }
    }
    input = temp;
} // fixup

QValidator::State HexValidator::validate(QString &input, int &pos) const
{
    if (!input.isEmpty()) {
        // TODO: can we detect if the char which was JUST deleted
        // (if any was deleted) was a space? and special case this?
        // as to not have the bug in this case?

        const int char_pos = pos - input.left(pos).count(' ');
        int chars = 0;
        fixup(input);
        pos = 0;
        while (chars != char_pos) {
            if (input[pos] != ' ')
                ++chars;
            ++pos;
        }

        // favor the right side of a space
        if (input[pos] == ' ')
            ++pos;
    }
    return QValidator::Acceptable;
} // validate
