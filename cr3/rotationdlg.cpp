#include "cr3widget.h"
#include "orientation.h"

#include "rotationdlg.h"
#include "ui_rotationdlg.h"

RotationDlg::RotationDlg(QWidget *parent, int an)
    : Dialog(parent)
    , ui(new Ui::RotationDlg)
    , angle(an)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );

    switch( angle )
    {
    case 0:
    default:
        ui->btnRot0->setFocus();
        break;
    case 1:
        ui->btnRot90->setFocus();
        break;
    case 2:
        ui->btnRot180->setFocus();
        break;
    case 3:
        ui->btnRot270->setFocus();
        break;
    }

    init();
}

RotationDlg::~RotationDlg()
{
    delete ui;
}

void RotationDlg::on_btnRot0_clicked()
{
    angle = 0;
    accept();
}

void RotationDlg::on_btnRot90_clicked()
{
    angle = 1;
    accept();
}

void RotationDlg::on_btnRot180_clicked()
{
    angle = 2;
    accept();
}

void RotationDlg::on_btnRot270_clicked()
{
    angle = 3;
    accept();
}
