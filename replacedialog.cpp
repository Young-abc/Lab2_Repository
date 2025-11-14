#include "replacedialog.h"
#include "ui_replacedialog.h"
#include "QMessageBox"

ReplaceDialog::ReplaceDialog(QWidget *parent,QPlainTextEdit* textEdit)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    pTextEdit = textEdit;
    ui->rbDown->setChecked(true);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

// 查找下一条
void ReplaceDialog::on_btFindNext_clicked()
{
    QString target = ui->searchText->text();

    if(target ==""||pTextEdit ==nullptr)
        return;

    QString text = pTextEdit->toPlainText();
    QTextCursor c = pTextEdit->textCursor();
    int index= -1;

    if(ui->rbDown->isChecked()){
        index = text.indexOf(target,c.position(),ui->cbCaseSensetive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);

        if(index>=0){
            c.setPosition(index);
            c.setPosition(index+target.length(),QTextCursor::KeepAnchor);

            pTextEdit->setTextCursor(c);
        }
    }else if(ui->rbUp->isChecked()){
        index = text.lastIndexOf(target,c.position()-text.length()-1,ui->cbCaseSensetive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);

        if(index>=0){
            c.setPosition(index+target.length());
            c.setPosition(index,QTextCursor::KeepAnchor);

            pTextEdit->setTextCursor(c);
        }
    }

    if(index <0){
        QMessageBox msg(this);
        msg.setWindowTitle("记事本");
        msg.setText(QString("找不到")+target);
        msg.setWindowFlag(Qt::Drawer);
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}

// 替换
void ReplaceDialog::on_btReplace_clicked()
{
    QString target = ui->searchText->text();
    QString to = ui->targetText->text();
    QString text = pTextEdit->toPlainText();

    QString selText = pTextEdit->textCursor().selectedText();

    if (target.isEmpty() || pTextEdit == nullptr) {
        QMessageBox::information(this, "提示", "请输入要替换的文本");
        return;
    }

    if(selText == target)
        pTextEdit->insertPlainText(to);
}

// 替换全部
void ReplaceDialog::on_btReplaceAll_clicked()
{
    QString target = ui->searchText->text();
    QString to = ui->targetText->text();

    if (target.isEmpty() || pTextEdit == nullptr) {
        QMessageBox::information(this, "提示", "请输入要替换的文本");
        return;
    }

    QString text = pTextEdit->toPlainText();
    Qt::CaseSensitivity caseSensitivity = ui->cbCaseSensetive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    // 统计替换次数
    int count = 0;
    int pos = 0;
    QString newText = text;

    // 循环查找并替换所有匹配内容
    while (true) {
        int index = newText.indexOf(target, pos, caseSensitivity);
        if (index == -1) break;

        newText.replace(index, target.length(), to);
        count++;
        pos = index + to.length();  // 从替换后的位置继续查找
    }

    if (count > 0) {
        // 将替换后的内容写回编辑器
        pTextEdit->setPlainText(newText);
        QMessageBox::information(this, "替换完成",
                                 QString("共替换 %1 处").arg(count));
    } else {
        QMessageBox::information(this, "提示",
                                 QString("找不到 '%1'").arg(target));
    }
}

// 取消按钮
void ReplaceDialog::on_btCancel_clicked()
{
    accept();
}

