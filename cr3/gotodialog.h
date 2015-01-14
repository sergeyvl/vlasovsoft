#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include "Dialog.h"

namespace Ui {
class GoToDialog;
}

class GoToDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit GoToDialog(QWidget *parent, int cp, int np);
    ~GoToDialog();

    int getCurrentPage() const
    { return cp_; }

private:
    int percentFromPageNo(int val) const;
    int pageNoFromPercent(int val) const;
    
private slots:
    void on_spbPage_valueChanged(int arg1);
    void on_progressBar_valueChanged(int value);
    void onInit();

private:
    bool initDone;
    int cp_, np_;
    Ui::GoToDialog *ui;
};

#endif // GOTODIALOG_H
