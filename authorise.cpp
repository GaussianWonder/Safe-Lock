#include "authorise.h"
#include "ui_authorise.h"

#include "encrypt.h"

#include <QMessageBox>
#include <fstream>
#include <string>

authorise::authorise(bool &authVal, int &minVal, QWidget *parent) :
    QDialog(parent),
    auth(authVal),
    min(minVal),
    ui(new Ui::authorise)
{
    ui->setupUi(this);
}

authorise::~authorise()
{
    delete ui;
}

void authorise::on_horizontalSlider_valueChanged(int value)
{
    ui->minValue->setText( QString::number(value) + QString(" (min)") );
}

void authorise::on_authBtn_clicked()
{
    if(ui->userInput->text().size() == 0 || ui->privateKeyInput->text().size() == 0 || ui->cyclesInput->text().size() == 0 || ui->cyclesInput->text().toInt() <= 0 || ui->cyclesInput->text().toInt() >= 16){
        QMessageBox::critical(this, "FAILED", "Invalid input, fill everything first\nThe secret number is supposed to be a number ranged between 1 and 15 (higher numbers will take longer time to compute)");
        return;
    }

    bool found = false, correct = false;
    std::fstream dataF("data.cbdat", std::ios::in);
    std::string str, keyword = "SAFEPAD";

    while(std::getline(dataF, str)){
        std::size_t pos = str.find("{:CBDATAUTH:}");
        try{
            if(pos != std::string::npos && keyword.compare(str.substr(0, pos)) == 0){
                found = true;
                std::string val = Encryption::oneWayCrypt(
                            ui->userInput->text().toLocal8Bit().constData(),
                            ui->privateKeyInput->text().toLocal8Bit().constData(),
                            ui->cyclesInput->text().toInt()
                    );
                std::string randval = str.substr(pos + 13, str.size() - (pos + 12));
                if(val.compare(str.substr(pos + 13, str.size() - (pos + 12))) == 0 ){
                    correct = true;
                    this->auth = true;
                    this->min = ui->horizontalSlider->value();
                    QMessageBox::information(this, "SUCCESS", "All done, you may use the app for " + ui->minValue->text() + "\n\nYou can close both dialogs");
                }
            }
        }
        catch(...){
            found = true;
            correct = false;
        }
    }

    if(!found){
        QMessageBox::StandardButton reply = QMessageBox::question(
                    this,
                    "DECISION TIME", "No auth content found, would you like to add one using these values\n[This means you also have to remember them]",
                    QMessageBox::Yes | QMessageBox::No
                );
        if(reply == QMessageBox::Yes){
            dataF.close();
            dataF.open("data.cbdat", std::ios_base::app);
            std::string val = Encryption::oneWayCrypt(
                        ui->userInput->text().toLocal8Bit().constData(),
                        ui->privateKeyInput->text().toLocal8Bit().constData(),
                        ui->cyclesInput->text().toInt()
                );

            dataF << keyword << "{:CBDATAUTH:}" << val << '\n';
            dataF.close();
            QMessageBox::information(this, "SUCCESS", "You may now press AUTHORISE again");
        }
    }
    else if(!correct) QMessageBox::critical(this, "FAILED", "Wrong input...");
}
