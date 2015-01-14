#ifndef SEARCHDLG_H
#define SEARCHDLG_H

#include "Dialog.h"

namespace Ui {
class SearchDialog;
}

class SearchDialog : public Dialog
{
    Q_OBJECT

public:
    SearchDialog(QWidget *parent);
    ~SearchDialog();

private:
    Ui::SearchDialog *ui;

private slots:
    void on_btnFind_clicked();
    void onSelectText();

public:
    static bool reverse;
    static bool caseSensitive;
    static QString lastText;
    QString text;
};

#endif // SEARCHDLG_H
