#ifndef ARITHMDIALOG_H
#define ARITHMDIALOG_H

#include <QDialog>

namespace Ui {
class ArithmDialog;
}

class ArithmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArithmDialog(QWidget *parent = nullptr);
    ~ArithmDialog();

    static bool doSelect(QWidget* parent, const QList<QString>& opNums1, const QList<QString>& opNums2,
                  QString& res1, QString& res2, int& ops, uint& flag);
    void showLists(const QList<QString>& opNums1, const QList<QString>& opNums2);

    QString getOps1() const;
    QString getOps2() const;
    int getOps() const;
    bool getDoCenterAligned() const;
    bool getDoScale() const;
    bool getDoUseMod() const;

private slots:
    void on_buttonDo_clicked(bool checked);

    void on_comboxArithmOpt_currentIndexChanged(int index);

private:
    Ui::ArithmDialog *ui;

    QString ops1;
    QString ops2;
    int ops;
    bool doCenterAligned;
    bool doScale;
    bool useMod;


};

#endif // ARITHMDIALOG_H
