#include "recentremovedlg.h"
#include "ui_recentremovedlg.h"

RecentRemoveDlg::RecentRemoveDlg(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::RecentRemoveDlg)
    , result(0)
{
    ui->setupUi(this);
    init();
}

RecentRemoveDlg::~RecentRemoveDlg()
{
    delete ui;
}

void RecentRemoveDlg::on_btnRecordOnly_clicked()
{
    result = 1;
    accept();
}

void RecentRemoveDlg::on_btnRecordAndFile_clicked()
{
    result = 2;
    accept();
}


void RecentRemoveDlg::on_btnCancel_clicked()
{
    result = 3;
    accept();
}
