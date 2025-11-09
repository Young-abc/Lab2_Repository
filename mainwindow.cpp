#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "qfiledialog.h"
#include "QMessageBox"
#include "QTextStream"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textChanged = false;
    on_actionNew_triggered();

    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length："+QString::number(0)+"  lines："+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("Ln："+QString::number(0)+"  Col："+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText("Name：杨泽仁");
    ui->statusbar->addPermanentWidget(author);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dig;
    dig.exec();
}


void MainWindow::on_actionFind_triggered()
{
    FindDialog dig;
    dig.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dig;
    dig.exec();
}


// 新建文件
void MainWindow::on_actionNew_triggered()
{
    if(textChanged){
        userEditConfirmed();
    }

    ui->TextEdit->clear();
    filePath.clear();
    this->setWindowTitle(tr("新建文本文件编辑器"));
    textChanged=false;
}


void MainWindow::on_actionOpen_triggered()
{
    // if()

    if(textChanged){
        userEditConfirmed();
    }

    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files (*.txt);;All(*.*)"));

    // 用户取消选择文件
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if(!file.open(QFile::ReadOnly|QFile::Text)){
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }


    // filePath = filename;

    QTextStream in(&file);
    QString text = in.readAll();
    ui->TextEdit->setPlainText(text);
    file.close();
    filePath = filename; //更新当前文件路径

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
    textChanged=false;
}


void MainWindow::on_actionSave_triggered()
{


    // 如果当前文件路径为空（新文件未保存过），则调用"另存为"
    if (filePath.isEmpty()) {
        on_actionSaveAs_triggered();
        return;
    }

    QFile file(filePath);

    if(!file.open(QFile::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,"..","打开文件失败");

        QString filename = QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files(*.txt)"));
        QFile file(filename);
        if(!file.open(QFile::WriteOnly|QFile::Text)){
            QMessageBox::warning(this,"..","打开保存文件失败");
            return;
        }
        filePath = filename;
    }

    QTextStream out(&file);
    QString text = ui->TextEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}


void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files (*.txt);;All(*.*)"));

    // 用户取消保存
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if(!file.open(QFile::ReadOnly|QFile::Text)){
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    filePath = filename;
    QTextStream out(&file);
    QString text = ui->TextEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}


void MainWindow::on_TextEdit_textChanged()
{
    if(!textChanged){
        this->setWindowTitle("*" + this->windowTitle());
        textChanged = true;
    }
}

bool MainWindow::userEditConfirmed()
{
    QString path = (filePath !="")?filePath:"无标题.txt";

    if(textChanged){
        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n")+"\"" + path + "\"?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel); // 保存/不保存/取消
        int result = msg.exec();
        switch (result) {
        case QMessageBox::Yes:
            on_actionSave_triggered();
            break;

        case QMessageBox::No:
            textChanged = false;
            break;

        case QMessageBox::Cancel:
            return false;
            break;
        }
    }
    return true;
}

