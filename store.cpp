#include "store.h"
#include "ui_store.h"

#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <string>

#include "encrypt.h"

store::store(QString PK, unsigned int C, QWidget *parent) :
    QDialog(parent),
    privateKey(PK),
    cycles(C),
    ui(new Ui::store)
{
    ui->setupUi(this);
}

store::~store()
{
    delete ui;
}

void store::on_saveBtn_clicked()
{
    if(ui->keywordInput->text().size() <= 0 || ui->data->toPlainText().size() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything please");
        return;
    }

    std::fstream file;
    std::string path = ui->pathInput->text().toUtf8().constData();
    if(path.size() == 0) path = "data.cbdat";

    file.open(path, std::ios_base::app);
    path = ui->data->toPlainText().toUtf8().constData();

    file << ui->keywordInput->text().toUtf8().constData()
         << "{:CBDATSEP:}"
         << Encryption::FullCrypt(
                path,
                this->privateKey.toUtf8().constData(),
                this->cycles
                )
         << '\n';
}

void store::on_showBtn_clicked()
{
    if(ui->keywordInput->text().size() <= 0){
        QMessageBox::critical(this, "OOPS", "Fill everything please");
        return;
    }

    std::fstream file;
    std::string path    = ui->pathInput->text().toUtf8().constData();
    std::string keyword = ui->keywordInput->text().toUtf8().constData();
    if(path.size() == 0) path = "data.cbdat";

    file.open(path, std::ios::in);

    while(std::getline(file, path)){
        std::size_t pos = path.find("{:CBDATSEP:}");

        if(pos != std::string::npos){
            if(keyword.compare(path.substr(0, pos)) == 0){
                QString result = QString::fromUtf8(Encryption::FullDecrypt(
                                                    path.substr(pos + 12, path.size() - (pos + 11)),
                                                    this->privateKey.toUtf8().constData(),
                                                    this->cycles
                                                    ).c_str()
                                                );

                ui->data->setText(ui->data->toPlainText() + '\n' + result);
            }
        }
        else QMessageBox::critical(this, "OOPS", "File seems to be corrupted...\nManually fix it by finding the missing separator {:CBDATSEP:}\nOr delete the file (data will be lost)\nFormat: KEYWORD + SEP + CRYPT\n");
    }
}

void store::on_browseBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("Choose file to encrypt"),
            "C://",
            "All files (*.*);;Text File (*.txt)"
        );

    ui->pathInput->setText( filePath );
}

void store::on_exitBtn_clicked()
{
    this->close();
}
