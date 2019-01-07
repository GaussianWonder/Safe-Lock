#include "stroperations.h"
#include "ui_stroperations.h"

#include <string>
#include "encrypt.h"

StrOperations::StrOperations(QString PK, unsigned int C, int S, QWidget *parent) :
    QDialog(parent),
    privateKey(PK),
    cycles(C),
    state(S),
    ui(new Ui::StrOperations)
{
    ui->setupUi(this);

    if(cycles < 10) ui->realTime->setCheckState(Qt::CheckState::Checked);
    else ui->realTime->setCheckState(Qt::CheckState::Unchecked);
}

StrOperations::~StrOperations()
{
    delete ui;
}

void StrOperations::setCryptedText()
{
    if(ui->stringInput->toPlainText().size() <= 0){
        return;
    }

    std::string str = ui->stringInput->toPlainText().toUtf8().constData();
    std::string privKey = this->privateKey.toUtf8().constData();

    if(state == 1)      str = Encryption::FullCrypt(str, privKey, this->cycles);
    else if(state == 0) str = Encryption::FullDecrypt(str, privKey, this->cycles);
    else if(state == -1)str = Encryption::oneWayCrypt(str, privKey, this->cycles);

    ui->stringOutput->setText( QString::fromUtf8(str.data(), str.size()) );
}

void StrOperations::on_cryptButton_clicked()
{
    setCryptedText();
}

void StrOperations::on_stringInput_textChanged()
{
    if(ui->realTime->checkState() == Qt::CheckState::Checked) setCryptedText();
}

void StrOperations::on_exitBtn_clicked()
{
    this->close();
}
