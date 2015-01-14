#ifndef BOOKMARKLISTDLG_H
#define BOOKMARKLISTDLG_H

#include "Dialog.h"
#include "lvptrvec.h"

namespace Ui {
    class BookmarkListDialog;
}

class QKeyEvent;
class QTableWidgetItem;
class CR3View;
class CRBookmark;

class BookmarkListDialog : public Dialog
{
    Q_OBJECT

public:
    BookmarkListDialog(QWidget *parent, CR3View * docView);
    ~BookmarkListDialog();

private:
    Ui::BookmarkListDialog* ui;
    CR3View* docview;
    LVPtrVector<CRBookmark>* pList;

private:
    CRBookmark* selectedBookmark();
    virtual void keyPressEvent(QKeyEvent* e);

private slots:
    virtual void accept();
    void on_btnDelete_clicked();
    void onClicked(QTableWidgetItem* pItem);
};

#endif // BOOKMARKLISTDLG_H
