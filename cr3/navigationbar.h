#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QAbstractScrollArea>

#include "gesturescontroller.h"

class QScrollBar;
class QLabel;

namespace Ui {
class NavigationBar;
}

class GesturesController;

class NavigationBar : public QWidget
{
    Q_OBJECT
    
public:
    NavigationBar(QWidget* parent = NULL);
    ~NavigationBar();
    
    void setControls( QAbstractScrollArea* ca, QLabel* pPos, const QSize& weights = QSize(1,1) );

public slots:
    void on_btnFirst_clicked();
    void on_btnPrev_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();  
    void onGesture(QPoint, GesturesController::GestureType t);

private slots:
    void onScrollValueChanged(int value);
    void onScrollRangeChanged(int min, int max);

private:
    Ui::NavigationBar *ui;
    QScrollBar* pSb_;
    QLabel* pPos_;

    GesturesController gc;
};

#endif // NAVIGATIONBAR_H
