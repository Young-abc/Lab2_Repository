#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "qfiledialog.h"
#include "QMessageBox"
#include "QTextStream"
#include "QColorDialog"
#include "QFontDialog"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionRecover->setEnabled(false);
    ui->actionReture->setEnabled(false);

    QPlainTextEdit::LineWrapMode mode = ui->TextEdit->lineWrapMode();

    if( mode == QTextEdit::NoWrap){
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionWrap->setChecked(false);
    } else{
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionWrap->setChecked(true);
    }

    textChanged = false;
    on_actionNew_triggered();

    statusLabel.setMaximumWidth(180);
    statusLabel.setText("length："+QString::number(0)+"  lines："+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(180);
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
    FindDialog dig(this,ui->TextEdit);
    dig.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dig(this,ui->TextEdit);
    dig.exec();
}


// 新建文件
void MainWindow::on_actionNew_triggered()
{
    if(textChanged){
        if(!userEditConfirmed())
            return;
    }

    ui->TextEdit->clear();
    filePath.clear();
    this->setWindowTitle(tr("新建文本文件编辑器"));
    textChanged=false;
}

//打开文件
void MainWindow::on_actionOpen_triggered()
{
    int flag = 0; //用于标记用户选择打开文件前文本修改状态
    if(textChanged){
        flag = 1;
        if(!userEditConfirmed())
            return; //当用户确认框选择取消时,停止打开文件操作
    }

    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files (*.txt);;All(*.*)"));

    // 用户取消选择文件
    if (filename.isEmpty()) {
        if(flag==1) textChanged=true; //当用户确认框选择no但又没有选择打开文件时,恢复文本原来状态
        return;
    }

    QFile file(filename);

    if(!file.open(QFile::ReadOnly|QFile::Text)){
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    ui->TextEdit->setPlainText(text);
    file.close();
    filePath = filename; //更新当前文件路径

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
    textChanged=false;
}

//保存文件
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


//另存为
void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files (*.txt);;All(*.*)"));

    // 用户取消保存
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if(!file.open(QFile::WriteOnly|QFile::Text)){
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

// 文本内容改变时处理函数
void MainWindow::on_TextEdit_textChanged()
{
    if(!textChanged){
        this->setWindowTitle("*" + this->windowTitle());
        textChanged = true;
    }

    // 更新状态栏显示的文本内容长度、行数
    statusLabel.setText("length："+QString::number(ui->TextEdit->toPlainText().length())+"  lines："+QString::number(ui->TextEdit->document()->lineCount()));
}

// 用户确认函数
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

// 撤销
void MainWindow::on_actionReture_triggered()
{
    ui->TextEdit->undo();
}

// 恢复
void MainWindow::on_actionRecover_triggered()
{
    ui->TextEdit->redo();
}

// 剪切
void MainWindow::on_actionCut_triggered()
{
    ui->TextEdit->cut();
    ui->actionPaste->setEnabled(true);
}

// 复制
void MainWindow::on_actionCopy_triggered()
{
    ui->TextEdit->copy();
    ui->actionPaste->setEnabled(true);
}

// 粘贴
void MainWindow::on_actionPaste_triggered()
{
    ui->TextEdit->paste();
}

// 全选
void MainWindow::on_actionSelectAll_triggered()
{
    ui->TextEdit->selectAll();
}


void MainWindow::on_TextEdit_redoAvailable(bool b)
{
    ui->actionRecover->setEnabled(b);
}


void MainWindow::on_TextEdit_undoAvailable(bool b)
{
    ui->actionReture->setEnabled(b);
}


void MainWindow::on_TextEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}

// 字体颜色
void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        QPalette palette = ui->TextEdit->palette();
        palette.setColor(QPalette::Text, color);
        ui->TextEdit->setPalette(palette);
    }
}

// 字体背景色（选中时的背景色）
void MainWindow::on_actionBgcolor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        QPalette palette = ui->TextEdit->palette();
        palette.setColor(QPalette::Highlight, color);  // 选中文本的背景色
        palette.setColor(QPalette::HighlightedText, ui->TextEdit->palette().color(QPalette::Text));
        ui->TextEdit->setPalette(palette);
    }
}

// 编辑器背景色
void MainWindow::on_actionEdiBgcolor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        QPalette palette = ui->TextEdit->palette();
        palette.setColor(QPalette::Base, color);
        ui->TextEdit->setPalette(palette);
    }
}

// 自动换行
void MainWindow::on_actionWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode = ui->TextEdit->lineWrapMode();

    if( mode == QTextEdit::NoWrap){
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionWrap->setChecked(true);
    } else{
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionWrap->setChecked(false);
    }
}

// 字体
void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok,this);

    if(ok)
        ui->TextEdit->setFont(font);
}

// 工具栏
void MainWindow::on_actionToolBar_triggered()
{
    bool isVisible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!isVisible);
    ui->actionToolBar->setChecked(!isVisible);
}

// 状态栏
void MainWindow::on_actionStatusBar_triggered()
{
    bool isVisible = statusBar()->isVisible();
    statusBar()->setVisible(!isVisible);
    ui->actionStatusBar->setChecked(!isVisible);
}


void MainWindow::on_actionExit_triggered()
{
    if(userEditConfirmed())
        exit(0);
}

// 更新状态栏显示的光标位置
void MainWindow::on_TextEdit_cursorPositionChanged()
{
    int col = 0;
    int ln = 0;
    int flg = -1;
    int pos = ui->TextEdit->textCursor().position();
    QString text = ui->TextEdit->toPlainText();

    for(int i = 0; i<pos;i++){
        if(text[i]=='\n'){
            ln++;
            flg = i;
        }
    }
    flg++;
    col = pos-flg;
    statusCursorLabel.setText("Ln："+QString::number(ln+1)+"  Col："+QString::number(col+1));
}

// 自动换行按钮
void MainWindow::on_actionShowLineNum_triggered()
{
    // 切换行号可见状态
    bool isVisible = ui->TextEdit->isLineNumberVisible();
    ui->TextEdit->setLineNumberVisible(!isVisible);

    // 更新菜单项的勾选状态
    ui->actionShowLineNum->setChecked(!isVisible);
}

