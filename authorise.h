#ifndef AUTHORISE_H
#define AUTHORISE_H

#include <QDialog>

namespace Ui {
class authorise;
}

class authorise : public QDialog
{
    Q_OBJECT

public:
    explicit authorise(bool &authVal, int &minVal, QWidget *parent = nullptr);
    ~authorise();

    bool &auth;
    int &min;
private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_authBtn_clicked();

private:
    Ui::authorise *ui;
};

#endif // AUTHORISE_H
