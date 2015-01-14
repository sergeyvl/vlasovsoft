#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenu>

class MainMenu : public QMenu
{
public:
    MainMenu(QWidget *parent = 0);
    
protected:
    virtual void mouseMoveEvent ( QMouseEvent* ) {}
};

#endif // MAINMENU_H
