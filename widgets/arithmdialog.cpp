#include "arithmdialog.h"
#include "ui_arithmdialog.h"

#include <QDebug>
#include <QMessageBox>
#include "lib/ops/arithmops.h"
#define debug(x)(qDebug() << (x))

ArithmDialog::ArithmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArithmDialog)
{
    ui->setupUi(this);

    // add arithm opt
    ui->comboxArithmOpt->addItem("加法运算");
    ui->comboxArithmOpt->addItem("减法运算");
    ui->comboxArithmOpt->addItem("乘法运算");
    ui->comboxArithmOpt->addItem("除法运算");
    ui->comboxArithmOpt->addItem("非运算");
    ui->comboxArithmOpt->addItem("交运算");
    ui->comboxArithmOpt->addItem("并运算");

    ui->listOpnum1->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listOpnum2->setSelectionMode(QAbstractItemView::SingleSelection);

    this->ops1 = "";
    this->ops2 = "";
    this->ops = 0;
}

ArithmDialog::~ArithmDialog()
{
    delete ui;
}

int ArithmDialog::getOps() const{
    return this->ops;
}

QString ArithmDialog::getOps1() const{
    return this->ops1;
}

QString ArithmDialog::getOps2() const{
    return this->ops2;
}

bool ArithmDialog::getDoScale() const{
    return this->doScale;
}

bool ArithmDialog::getDoCenterAligned() const{
    return this->doCenterAligned;
}

bool ArithmDialog::getDoUseMod() const{
    return this->useMod;
}

void ArithmDialog::on_buttonDo_clicked(bool checked)
{
    // 考虑用信号传递参数
    QList<QListWidgetItem *> op1s = ui->listOpnum1->selectedItems();
    QList<QListWidgetItem *> op2s = ui->listOpnum2->selectedItems();
    if(!op1s.isEmpty()){
        QString op1 = op1s[0]->text();
        debug(op1);
        this->ops1 = op1;
    }else{
        QMessageBox::about(this, tr("提示"), tr("请至少选择操作图像1"));
        return ;
    }
    if(!op2s.isEmpty()){
        QString op2 = op2s[0]->text();
        debug(op2);
        this->ops2 = op2;
    }else{
        if(ops != 4){// 处理双目运算，第二个数未指定的情况。
            QMessageBox::about(this, tr("提示"), tr("指定的运算非单目运算，请选择处理图像2"));
            return ;
        }
    }

    int ops = ui->comboxArithmOpt->currentIndex();
    debug(ops);
    this->ops = ops;
    this->doCenterAligned = ui->checkDoCenterAligned->isChecked();
    this->doScale = ui->checkDoRescale->isChecked();
    this->useMod = ui->checkDoUseMod->isChecked();

    QDialog::accept();
}

void ArithmDialog::showLists(const QList<QString> &opNums1, const QList<QString> &opNums2){
    for(QString str: opNums1){
        ui->listOpnum1->addItem(str);
    }
    for(QString str: opNums2){
        ui->listOpnum2->addItem(str);
    }
}

bool ArithmDialog::doSelect(QWidget* parent, const QList<QString> &opNums1, const QList<QString> &opNums2,
                            QString &res1, QString &res2, int &ops, uint& flag){


    ArithmDialog dialog(parent);
    dialog.showLists(opNums1, opNums2);
    if( dialog.exec() == QDialog::Accepted){
        res1 = dialog.getOps1();
        res2 = dialog.getOps2();
        ops = dialog.getOps();
        flag = ArithmOps::constructFlag(
                    dialog.getDoCenterAligned(),
                    dialog.getDoScale(),
                    dialog.getDoUseMod());
        debug(flag);
        return true;
    }else{
        return false;
    }
}

void ArithmDialog::on_comboxArithmOpt_currentIndexChanged(int index)
{
    // 单目运算时禁止第二个列表选择
    if(index != 4){
        ui->listOpnum2->setSelectionMode(QAbstractItemView::SingleSelection);
    }else{
        ui->listOpnum2->setSelectionMode(QAbstractItemView::NoSelection);
    }
}
