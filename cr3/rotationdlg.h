#ifndef ROTATIONDLG_H
#define ROTATIONDLG_H

#include "Dialog.h"

namespace Ui {
class RotationDlg;
}

class RotationDlg : public Dialog
{
    Q_OBJECT
    
public:
    RotationDlg(QWidget *parent, int an);
    ~RotationDlg();
    
private slots:
    void on_btnRot0_clicked();
    void on_btnRot90_clicked();
    void on_btnRot180_clicked();
    void on_btnRot270_clicked();

private:
    int angle;
    Ui::RotationDlg *ui;

public:
    int getRotation() const
    { return angle; }
};

#endif // ROTATIONDLG_H
