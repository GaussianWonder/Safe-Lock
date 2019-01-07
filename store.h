#ifndef STORE_H
#define STORE_H

#include <QDialog>

namespace Ui {
class store;
}

class store : public QDialog
{
    Q_OBJECT

public:
    explicit store(QString PK, unsigned int C, QWidget *parent = nullptr);
    ~store();

private slots:
    void on_saveBtn_clicked();

    void on_showBtn_clicked();

    void on_browseBtn_clicked();

private:
    Ui::store *ui;
    QString privateKey;
    unsigned int cycles;
};

#endif // STORE_H
