#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void setTime();
    void blockAccess();
    void grantAccess();

private slots:
    void on_verticalSlider_sliderMoved(int position);

    void on_cycleSlider_sliderMoved(int position);

    void on_cycleSlider_valueChanged(int value);

    void on_saveSettings_clicked();

    void on_loadSettings_clicked();

    void on_actionFileEncrypt_triggered();

    void on_actionFileDecrypt_triggered();

    void on_actionStore_triggered();

    void on_actionAuthorise_triggered();

    void on_actionOne_Way_triggered();

private:
    Ui::MainWindow *ui;
    QTimer *authTimer;
    bool isAuth;
    unsigned int time;
};

#endif // MAINWINDOW_H
