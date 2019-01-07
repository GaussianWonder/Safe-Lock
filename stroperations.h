#ifndef STROPERATIONS_H
#define STROPERATIONS_H

#include <QDialog>

namespace Ui {
class StrOperations;
}

class StrOperations : public QDialog
{
    Q_OBJECT

public:
    explicit StrOperations(QString PK, unsigned int C, int S, QWidget *parent = nullptr);
    ~StrOperations();

public slots:
    void setCryptedText();

private slots:
    void on_cryptButton_clicked();

    void on_stringInput_textChanged();

    void on_exitBtn_clicked();

private:
    Ui::StrOperations *ui;
    QString privateKey;
    unsigned int cycles;
    int state;
};

#endif // STROPERATIONS_H
