#ifndef RecentBooksDlg_H
#define RecentBooksDlg_H

#include "Dialog.h"

#include <math.h>

#include "cr3widget.h"
#include "crqtutil.h"
#include "lvdocview.h"
#include "settings.h"

namespace Ui {
class RecentBooksDlg;
}

class QTableWidgetItem;
class CR3View;

class RecentBooksDlg : public Dialog
{
    Q_OBJECT
    Q_DISABLE_COPY(RecentBooksDlg)  

public:
    RecentBooksDlg(QWidget *parent, CR3View * docView);
    virtual ~RecentBooksDlg();

private:
    Ui::RecentBooksDlg* ui;
    CR3View* docview;
    int getBookNum();

private:
    virtual void keyPressEvent(QKeyEvent* e);

private slots:
    virtual void accept();
    void on_btnOk_clicked();
    void on_btnFolder_clicked();
    void on_btnRemove_clicked();
    void onClicked(QTableWidgetItem*);

public:
    QString fileName;
};

#endif // RecentBooksDlg_H
