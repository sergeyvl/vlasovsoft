#ifndef RECENTREMOVEDLG_H
#define RECENTREMOVEDLG_H

#include "Dialog.h"

namespace Ui {
class RecentRemoveDlg;
}

class RecentRemoveDlg : public Dialog
{
    Q_OBJECT
    
public:
    RecentRemoveDlg(QWidget *parent = 0);
    ~RecentRemoveDlg();

    int getResult() const
    { return result; }
    
private slots:
    void on_btnRecordOnly_clicked();
    void on_btnRecordAndFile_clicked();
    void on_btnCancel_clicked();

private:
    Ui::RecentRemoveDlg *ui;
    int result;
};

#endif // RECENTREMOVEDLG_H
