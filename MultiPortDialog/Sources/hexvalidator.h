#ifndef HEXVALIDATOR_H
#define HEXVALIDATOR_H

#include <QValidator>

class HexValidator : public QValidator {
    Q_OBJECT

public:
    explicit HexValidator(QObject * parent);
    void fixup(QString &input) const;
    State validate(QString &input, int &pos) const;

signals:
public slots:
};
#endif // HEXVALIDATOR_H
