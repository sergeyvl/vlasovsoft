#ifndef FRONTLIGHTDLG_H
#define FRONTLIGHTDLG_H

#include <QWidget>

namespace Ui {
class FrontLightDlg;
}

class FrontLightDlg : public QWidget
{
    Q_OBJECT
    
public:
    explicit FrontLightDlg(QWidget *parent = 0);
    ~FrontLightDlg();
    
private slots:
    void on_btnFrontlightDown_clicked();
    void on_btnFrontlightUp_clicked();

private:
    Ui::FrontLightDlg *ui;

    void setFrontLightLevel( int val );
};

#endif // FRONTLIGHTDLG_H
