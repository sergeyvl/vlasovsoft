#include <QLabel>
#include <QScrollBar>

#include "gesturescontroller.h"

#include "navigationbar.h"
#include "ui_navigationbar.h"

class MyScrollBar: public QScrollBar
{
public:
    MyScrollBar( QWidget * parent = 0 )
        : QScrollBar(parent)
    {}
protected:
    void sliderChange ( SliderChange change )
    {
        if ( SliderStepsChange == change )
            emit rangeChanged( minimum(), maximum() );
        QScrollBar::sliderChange(change);
    }
};

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NavigationBar)
    , pSb_(NULL)
{
    ui->setupUi(this);

    setVisible(false);
}

NavigationBar::~NavigationBar()
{
    delete ui;
}

void NavigationBar::setControls( QAbstractScrollArea* sa, QLabel* pPos, const QSize& weights )
{
    if ( sa )
    {
        sa->setVerticalScrollBar(new MyScrollBar(sa));
        pSb_  = sa->verticalScrollBar();
        pPos_ = pPos;
        gc.setWidget( sa->viewport() );
        gc.setWeights( weights );
        QObject::connect( pSb_, SIGNAL(valueChanged(int)), this, SLOT(onScrollValueChanged(int)) );
        QObject::connect( pSb_, SIGNAL(rangeChanged(int, int)), this, SLOT(onScrollRangeChanged(int, int)) );
        QObject::connect( &gc, SIGNAL(sigGesture(QPoint,GesturesController::GestureType)), this, SLOT(onGesture(QPoint,GesturesController::GestureType)) );
        onScrollRangeChanged(pSb_->minimum(),pSb_->maximum());
    }
}

void NavigationBar::on_btnFirst_clicked()
{
    if ( pSb_ ) pSb_->setValue( pSb_->minimum() );
}

void NavigationBar::on_btnPrev_clicked()
{
    if ( pSb_ )
    {
        int val = pSb_->value() - pSb_->pageStep();
        if ( val < pSb_->minimum() ) val = pSb_->minimum();
        pSb_->setValue( val);
    }
}

void NavigationBar::on_btnNext_clicked()
{
    if ( pSb_ )
    {
        int val = pSb_->value() + pSb_->pageStep();
        if ( val > pSb_->maximum() ) val = pSb_->maximum();
        pSb_->setValue( val);
    }
}

void NavigationBar::on_btnLast_clicked()
{
    if ( pSb_ ) pSb_->setValue( pSb_->maximum() );
}

void NavigationBar::onScrollValueChanged(int value)
{
    if ( pPos_ )
    {
        int max = pSb_->maximum();
        int ps = pSb_->pageStep();
        pPos_->setText( max > 0? QString("%1-%2/%3 (%4%)").arg(value+1).arg(value+ps).arg(max+ps).arg((value+ps)*100/(max+ps)):"");
    }
}

void NavigationBar::onScrollRangeChanged(int min, int max)
{
    Q_UNUSED(min)
    if ( pPos_ )
    {
        int value = pSb_->value();
        int ps = pSb_->pageStep();
        setVisible( max > 0 );
        pPos_->setVisible( max > 0 );
        pPos_->setText( max > 0? QString("%1-%2/%3 (%4%)").arg(value+1).arg(value+ps).arg(max+ps).arg((value+ps)*100/(max+ps)):"");
    }
}

void NavigationBar::onGesture(QPoint, GesturesController::GestureType t)
{
    switch ( t )
    {
    case GesturesController::gtSwipeBT:
        on_btnNext_clicked();
        break;
    case GesturesController::gtSwipeTB:
        on_btnPrev_clicked();
        break;
    case GesturesController::gtSwipeRL:
        on_btnLast_clicked();
        break;
    case GesturesController::gtSwipeLR:
        on_btnFirst_clicked();
        break;
    default:
        break;
    }
}
