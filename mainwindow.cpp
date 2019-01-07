#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "authorise.h"
#include "store.h"
#include "stroperations.h"

#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include "encrypt.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::blockAccess()
{
    ui->privateKeyInput->setEnabled(false);
    ui->cyclesInput->setEnabled(false);
    ui->cycleSlider->setEnabled(false);
    ui->saveSettings->setEnabled(false);
    ui->loadSettings->setEnabled(false);

    ui->logArea->setText("ACCESS WAS BLOCKED, " + QString::number(time) + " minutes passed\nPlease reauthorise");
    authTimer->stop();
    disconnect(authTimer, SIGNAL(timeout()), this, SLOT(blockAccess()));

    ui->privateKeyInput->setText("");
    ui->cyclesInput->setText("");
}

void MainWindow::grantAccess()
{
    ui->privateKeyInput->setEnabled(true);
    ui->cyclesInput->setEnabled(true);
    ui->cycleSlider->setEnabled(true);
    ui->saveSettings->setEnabled(true);
    ui->loadSettings->setEnabled(true);
}

void MainWindow::setTime()
{
    authTimer = new QTimer(this);
    connect(authTimer, SIGNAL(timeout()), this, SLOT(blockAccess()));
    grantAccess();
    authTimer->start(this->time * 60000);
}

void MainWindow::on_verticalSlider_sliderMoved(int position)
{
    //This one is broken wtf?
    position = position + 1 - 1;
}

void MainWindow::on_cycleSlider_sliderMoved(int position)
{
    ui->cyclesInput->setText( QString::number(position) );
}

void MainWindow::on_cycleSlider_valueChanged(int value)
{   //Both do the same god damn thing, why did i bother creating 2? idk
    ui->cyclesInput->setText( QString::number(value) );
}

void MainWindow::on_saveSettings_clicked()
{
    //Save the current configuration
    SettingsReader::readData();
    std::string log = SettingsReader::setData(
        3,
        ui->privateKeyInput->text().toLocal8Bit().constData(),
        ui->cyclesInput->text().toInt()
    );

    ui->logArea->setText( QString::fromUtf8(log.data(), log.size()) );
}

void MainWindow::on_loadSettings_clicked()
{
    //Load the one from settings.cbdat
    SettingsReader::readData();
    std::string log = "Settings were read and are now displayed on the screen";

    ui->privateKeyInput->setText( QString::fromUtf8(SettingsReader::key.data(),
                                                    SettingsReader::key.size())
                                );
    ui->cyclesInput->setText( QString::number(SettingsReader::cycles) );
    ui->cycleSlider->setValue( SettingsReader::cycles );

    ui->logArea->setText( QString::fromUtf8(log.data(), log.size()) );
}

void MainWindow::on_actionFileEncrypt_triggered()
{
    if(ui->privateKeyInput->text().size() <= 0 || ui->cyclesInput->text().toInt() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything or authorise");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("Choose file to encrypt"),
            "C://",
            "All files (*.*);;Text File (*.txt)"
        );

    if(filePath.size() <= 1){
        ui->logArea->setText( "Invalid path, encryption aborted\n" );
        return;
    }
    else if(ui->privateKeyInput->text().size() == 0 || ui->cyclesInput->text().size() == 0){
        ui->logArea->setText( "Invalid input, try these:\n1) Provide valid input\n2)Load from a saved state\n3)Authorise if you can`t do 1) or 2)\n" );
    }

    std::string log = Encryption::CryptFile(
                filePath.toLocal8Bit().constData(),
                ui->privateKeyInput->text().toLocal8Bit().constData(),
                ui->cyclesInput->text().toInt()
        );

    ui->logArea->setText( QString::fromUtf8(log.data(), log.size()) );
}

void MainWindow::on_actionFileDecrypt_triggered()
{
    if(ui->privateKeyInput->text().size() <= 0 || ui->cyclesInput->text().toInt() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything or authorise");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("Choose file to encrypt"),
            "C://",
            "All files (*.*);;Text File (*.txt)"
        );

    if(filePath.size() <= 1){
        ui->logArea->setText( "Invalid path, decryption aborted\n" );
        return;
    }
    else if(ui->privateKeyInput->text().size() == 0 || ui->cyclesInput->text().size() == 0){
        ui->logArea->setText( "Invalid input, try these:\n1) Provide valid input\n2)Load from a saved state\n3)Authorise if you can`t do 1) or 2)\n" );
    }

    std::string log = Encryption::DecryptFile(
                filePath.toLocal8Bit().constData(),
                ui->privateKeyInput->text().toLocal8Bit().constData(),
                ui->cyclesInput->text().toInt()
        );

    ui->logArea->setText( QString::fromUtf8(log.data(), log.size()) );
}

void MainWindow::on_actionStore_triggered()
{
    if(ui->privateKeyInput->text().size() <= 0 || ui->cyclesInput->text().toInt() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything or authorise");
        return;
    }

    QString privateKey  = ui->privateKeyInput->text();
    unsigned int cycles = ui->cycleSlider->value();

    store storeW(privateKey, cycles);
    storeW.setModal(true);
    storeW.exec();

    ui->logArea->setText("Return from store action.\n");
}

void MainWindow::on_actionAuthorise_triggered()
{
    ui->logArea->setText( "Starting auth in a modal window\n" );
    bool auth = false;
    int min = 0;

    authorise authW(auth, min);
    authW.setModal(true);
    authW.exec();

    if(auth == true && min > 0){
        ui->logArea->setText( "Exiting auth\nBack in the main window\nAuth Success\n" + QString::number(min) + " min spanlife\n");
        isAuth = true;
        time = min;
        setTime();
    }
    else ui->logArea->setText( "Exiting auth\nBack in the main window\nAuth Failed" );
}

void MainWindow::on_actionOne_Way_triggered()
{   //QString PK, unsigned int C, int S, QWidget *parent = nullptr
    if(ui->privateKeyInput->text().size() <= 0 || ui->cyclesInput->text().toInt() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything or authorise");
        return;
    }

    StrOperations strOpW(ui->privateKeyInput->text(), ui->cyclesInput->text().toInt(), -1);
    strOpW.setModal(true);
    strOpW.exec();
}

void MainWindow::on_actionStringEncrypt_triggered()
{
    if(ui->privateKeyInput->text().size() <= 0 || ui->cyclesInput->text().toInt() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything or authorise");
        return;
    }

    StrOperations strOpW(ui->privateKeyInput->text(), ui->cyclesInput->text().toInt(), 1);
    strOpW.setModal(true);
    strOpW.exec();
}

void MainWindow::on_actionStringDecrypt_triggered()
{
    if(ui->privateKeyInput->text().size() <= 0 || ui->cyclesInput->text().toInt() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything or authorise");
        return;
    }

    StrOperations strOpW(ui->privateKeyInput->text(), ui->cyclesInput->text().toInt(), 0);
    strOpW.setModal(true);
    strOpW.exec();
}
