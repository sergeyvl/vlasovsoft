#include <QDir>
#include <QItemDelegate>
#include <QPainter>
#include <QFile>

#include "QtUtils.h"

#include "props.h"
#include "openfiledlg.h"
#include "recentremovedlg.h"

#include "recentdlg.h"
#include "ui_recentdlg.h"

class RecentTableWidgetItem: public QTableWidgetItem
{
public:
    QString title;
    QString info;
    QString percent;
};

class RecentBooksListDelegate : public QItemDelegate
{
    QFont fontTitle;
    QFont fontInfo;

public:
    RecentBooksListDelegate(QObject* pobj, const QFont& ft, const QFont& fi)
        : QItemDelegate(pobj)
        , fontTitle(ft)
        , fontInfo(fi)
    {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem newOption;
        newOption = option;
        newOption.state &= ~QStyle::State_HasFocus;
        newOption.palette.setColor( QPalette::Disabled, QPalette::Text, Qt::black );

        QTableWidget* pWidget = static_cast<QTableWidget*>( parent() );
        RecentTableWidgetItem* pItem = static_cast<RecentTableWidgetItem*>(pWidget->item(index.row(), index.column()));

        painter->setFont(fontTitle);
        QFontMetrics tfm(fontTitle);
        int fontTitleSize = tfm.height();
        QRect titleRect( newOption.rect.x(), newOption.rect.y(), newOption.rect.width(), fontTitleSize );
        painter->drawText( titleRect, Qt::TextSingleLine, tfm.elidedText(pItem->title, Qt::ElideRight, newOption.rect.width() ));

        painter->setFont(fontInfo);
        QFontMetrics ifm(fontInfo);
        int fontAuthorSize = ifm.height();
        QRect infoRect( newOption.rect.x(), newOption.rect.y() + fontTitleSize, newOption.rect.width(), fontAuthorSize );

        int percentWidth = ifm.boundingRect(pItem->percent).width();
        infoRect.adjust(0,0,-percentWidth,0);

        painter->drawText( infoRect, Qt::TextSingleLine, ifm.elidedText(pItem->info, Qt::ElideRight, infoRect.width() ) );

        QRect percentRect( infoRect.right(), infoRect.top(), percentWidth, infoRect.height() );
        painter->drawText( percentRect, Qt::TextSingleLine, pItem->percent );

        if (index.flags() & Qt::ItemIsSelectable)
        {
            Qt::GlobalColor lineColor = Qt::black;
            int lineWidth = newOption.state & QStyle::State_Selected ? 5:0;
            QPen pen(lineColor, lineWidth, Qt::SolidLine);
            painter->setPen(pen);
            painter->drawLine(newOption.rect.left(), newOption.rect.bottom()-lineWidth/2, newOption.rect.right(), newOption.rect.bottom()-lineWidth/2);
        }
    }
};

RecentBooksDlg::RecentBooksDlg(QWidget *parent, CR3View * docView )
    : Dialog(parent)
    , ui(new Ui::RecentBooksDlg)
    , docview(docView)
{
    ui->setupUi(this);

    QFont f(ui->tableWidget->font());
    f.setPointSize(docview->getOptions()->getIntDef("cr3.app.ui.recent.font.size", 12));
    ui->tableWidget->setFont(f);

    f = ui->lblPosition->font();
    f.setPointSize(f.pointSize()/2);
    ui->lblPosition->setFont(f);

    f = ui->lblTitle->font();
    f.setBold(true);
    ui->lblTitle->setFont(f);

    ui->tableWidget->setColumnCount(1);

    ui->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Custom);

    bool isVscroll = docview->getOptions()->getIntDef(PROP_APP_UI_RECENT_VSCROLLBAR, 1) != 0;
    ui->tableWidget->setVerticalScrollBarPolicy(isVscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    bool isHscroll = docview->getOptions()->getIntDef(PROP_APP_UI_RECENT_HSCROLLBAR, 1) != 0;
    ui->tableWidget->setHorizontalScrollBarPolicy(isHscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);

    // fill rows
    QFont fontTitle = ui->tableWidget->font();
    fontTitle.setBold(true);

    QFont fontAuthor = ui->tableWidget->font();
    fontAuthor.setItalic(true);

    QFont fontInfo = ui->tableWidget->font();
    fontInfo.setItalic(true);
    fontInfo.setPointSize(fontInfo.pointSize()*2/3);

    QFontMetrics tfm(fontTitle);
    QFontMetrics afm(fontAuthor);

    int rowHeight = tfm.height() + afm.height();
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(rowHeight);

    ui->tableWidget->setItemDelegate(new RecentBooksListDelegate(ui->tableWidget, fontTitle, fontAuthor));

    LVPtrVector<CRFileHistRecord>& files = docview->getDocView()->getHistory()->getRecords();

    // remove non-existing entries
    for( int k=0; k<files.length(); )
    {
        QString fn(cr2qt(files.get(k)->getFilePathName()));
        if (!QFile::exists(fn))
            files.remove(k);
        else
            ++k;
    }

    ui->tableWidget->setRowCount( files.length() );

    // insert items
    for( int index=0; index<files.length(); ++index )
    {
        CRFileHistRecord* book = files.get(index);

        RecentTableWidgetItem* titleItem = new RecentTableWidgetItem();
        titleItem->title   = cr2qt(book->getTitle());
        if ( titleItem->title.isEmpty() )
            titleItem->title = cr2qt(book->getFileName());
        titleItem->info  = cr2qt(book->getAuthor());
        if ( titleItem->info.isEmpty() )
            titleItem->info = "-";
        titleItem->percent = " " + crpercent(book->getLastPos()->getPercent());

        ui->tableWidget->setItem(index, 0, titleItem);
    }

    ui->navBar->setControls(ui->tableWidget, ui->lblPosition,
                            QSize( docview->getOptions()->getIntDef(PROP_APP_UI_SWIPES_X_WEIGHT, 1),
                                   docview->getOptions()->getIntDef(PROP_APP_UI_SWIPES_Y_WEIGHT, 1) ) );

    ui->tableWidget->setEditFocus(true);
    ui->tableWidget->setCurrentCell(0,0);

    QObject::connect( ui->tableWidget, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(onClicked(QTableWidgetItem*)) );
    QObject::connect( ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onClicked(QTableWidgetItem*)) );

    init();
}

RecentBooksDlg::~RecentBooksDlg()
{
    delete ui;
}

int RecentBooksDlg::getBookNum()
{
    return ui->tableWidget->currentRow();
}

void RecentBooksDlg::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {
    case Qt::Key_Menu:
        if ( ui->tableWidget->hasFocus() )
            ui->btnFolder->setFocus();
        else
            ui->tableWidget->setEditFocus(true);
        break;
    default:
        break;
    }
}

void RecentBooksDlg::accept()
{
    int bn = getBookNum();
    if ( bn >= 0 )
    {
        LVPtrVector<CRFileHistRecord>& files = docview->getDocView()->getHistory()->getRecords();
        fileName = cr2qt(files[bn]->getFilePathName());
        QDialog::accept();
    }
}

void RecentBooksDlg::on_btnOk_clicked()
{
    accept();
}

void RecentBooksDlg::on_btnFolder_clicked()
{
    int bn = getBookNum();
    if ( bn >= 0 )
    {
        LVPtrVector<CRFileHistRecord>& files = docview->getDocView()->getHistory()->getRecords();
        QString path(cr2qt(files[bn]->getFilePath()));
        OpenFileDlg dlg( this, docview, path );
        if ( dlg.exec() && !dlg.fileName.isEmpty() )
        {
            fileName = dlg.fileName;
            QDialog::accept();
        }
    }
}

void RecentBooksDlg::on_btnRemove_clicked()
{
    int bn = getBookNum();
    if ( bn >= 0 )
    {
        LVPtrVector<CRFileHistRecord>&
                files = docview->getDocView()->getHistory()->getRecords();

        int result = 0;
        int value  = docview->getOptions()->getIntDef(PROP_APP_UI_RECENT_REMOVE_FILES, 0);
        if ( value )
        {
            RecentRemoveDlg dlg(this);
            dlg.exec(false);
            result = dlg.getResult();
        }
        else
        {
            result = 1;
        }

        switch( result )
        {
        case 0:
        default:
            break;
        case 1:
            if ( bn > 0 || !docview->getDocView()->isDocumentOpened() )
            {
                files.remove(bn);
                ui->tableWidget->removeRow(bn);
            }
            break;
        case 2:
            if ( 0 == bn && docview->getDocView()->isDocumentOpened() )
            {
                docview->getDocView()->createDefaultDocument( qt2cr(tr("Removed")), qt2cr(tr("Document was removed") ) );
            }
            {
                QString fileName(cr2qt(files[bn]->getFilePathName()));
                files.remove(bn);
                ui->tableWidget->removeRow(bn);
                QFile::remove(fileName);
                docview->update();
            }
            break;
        }
    }
}

void RecentBooksDlg::onClicked(QTableWidgetItem*)
{
    accept();
}
