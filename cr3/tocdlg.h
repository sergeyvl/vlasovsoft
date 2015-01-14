#ifndef TOCDLG_H
#define TOCDLG_H

#include "Dialog.h"

#include "lvdocview.h"
#include "crqtutil.h"

namespace Ui {
class TocDlg;
}

class QTreeWidgetItem;
class CR3View;

class TocDlg : public Dialog
{
    Q_OBJECT

public:
    TocDlg(QWidget *parent, CR3View * docView);
    virtual ~TocDlg();

private:
    Ui::TocDlg* ui;
    CR3View* docview;

private:
    virtual void keyPressEvent(QKeyEvent* e);

private slots:
    virtual void accept();
    void onClicked( QTreeWidgetItem* pItem, int );
};

#endif // TOCDLG_H
