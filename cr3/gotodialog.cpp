#include "QtUtils.h"
#include "progbareventfilter.h"

#include "gotodialog.h"
#include "ui_gotodialog.h"

GoToDialog::GoToDialog(QWidget* parent, int cp, int np)
    : Dialog(parent)
    , initDone(false)
    , cp_(cp)
    , np_(np)
    , ui(new Ui::GoToDialog)
{
    ui->setupUi(this);

    QFont f(ui->lblTitle->font());
    f.setBold(true);
    ui->lblTitle->setFont(f);

    ui->lblTotal->setText(QString("/ %1").arg(np_));
    ui->progressBar->installEventFilter(new ProgBarEventFilter(this));

    ui->spbPage->setMinimum(1);
    ui->spbPage->setMaximum(np_);
    ui->spbPage->setValue(cp_);

    ui->progressBar->setMinimum(1);
    ui->progressBar->setMaximum(np_);
    ui->progressBar->setFormat("%p%");
    ui->progressBar->setValue(cp_);

    initDone = true;

    init();

    QTimer::singleShot(0, this, SLOT(onInit()));
}

GoToDialog::~GoToDialog()
{
    delete ui;
}

static int round( float val )
{
    return (val > 0.0) ? (val + 0.5) : (val - 0.5);
}

int GoToDialog::percentFromPageNo(int val) const
{
    return round( 100.0 * val/np_ );
}

int GoToDialog::pageNoFromPercent(int val) const
{
    return round( np_ * val / 100.0 );
}

void GoToDialog::on_spbPage_valueChanged(int arg1)
{
    if ( initDone )
    {
        cp_ = arg1;
        if ( sender() != ui->progressBar )
            ui->progressBar->setValue(arg1);
    }
}

void GoToDialog::on_progressBar_valueChanged(int value)
{
    if ( initDone )
    {
        cp_ = value;
        if ( sender() != ui->spbPage )
            ui->spbPage->setValue( cp_);
    }
}

void GoToDialog::onInit()
{
    ui->spbPage->setEditFocus(true);
}
