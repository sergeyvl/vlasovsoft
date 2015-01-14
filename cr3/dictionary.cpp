#include "props.h"
#include "cr3widget.h"

#include "ui_dictionary.h"
#include "dictionary.h"

Dictionary::Dictionary(CR3View* v, QWidget *parent)
    : QWidget(parent)
    , isFontDone(false)
    , ui(new Ui::Dictionary)
    , view(v)
{
    ui->setupUi(this);
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );
}

Dictionary::~Dictionary()
{
    delete ui;
}

void Dictionary::on_btnClose_clicked()
{
    view->clearSelection();
    hide();
}

void Dictionary::translate ( const QString& phrase )
{
    ui->widget->translate( phrase );
}

void Dictionary::moveDown()
{
    resize( view->width(), view->height()*45/100 );
    move( 0, view->height()-height() );
    ui->btnUpDown->setIcon( QIcon(":/icons/arrow_top.png") );
}

void Dictionary::moveUp()
{
    resize( view->width(), view->height()*45/100 );
    move( 0, 0 );
    ui->btnUpDown->setIcon( QIcon(":/icons/arrow_bottom.png") );
}

void Dictionary::on_btnPrevPage_clicked()
{
    view->clearSelection();
    QPoint p(pos());
    if ( 0 == p.y() )
    {
        moveDown();
    }
    else
    {
        view->doCommand(DCMD_PAGEUP, 1);
        moveUp();
    }
}

void Dictionary::on_btnNextPage_clicked()
{
    view->clearSelection();
    QPoint p(pos());
    if ( 0 == p.y() )
    {
        view->doCommand(DCMD_PAGEDOWN, 1);
        moveDown();
    }
    else
    {
        moveUp();
    }
}

void Dictionary::on_btnUpDown_clicked()
{
    QPoint pt(pos());
    if ( 0 == pt.y() )
        moveDown();
    else
        moveUp();
}

void Dictionary::show()
{
    if ( !isFontDone )
    {
        QFont f(ui->widget->font());
        f.setPointSize(view->getOptions()->getIntDef(PROP_APP_UI_DICT_FONT_SIZE, 12));
        ui->widget->setFont(f);
        isFontDone = true;
    }
    QWidget::show();
}
