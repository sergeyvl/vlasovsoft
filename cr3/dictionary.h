#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QWidget>

namespace Ui {
class Dictionary;
}

class CR3View;

class Dictionary : public QWidget
{
    Q_OBJECT

    bool isFontDone;
    Ui::Dictionary* ui;
    CR3View* view;

public:
    Dictionary(CR3View* v, QWidget *parent = 0);
    ~Dictionary();

private slots:
    void on_btnClose_clicked();
    void on_btnPrevPage_clicked();
    void on_btnNextPage_clicked();
    void on_btnUpDown_clicked();

public:
    void show();
    void translate ( const QString& phrase );
    void moveDown();
    void moveUp();
};

#endif // DICTIONARY_H
