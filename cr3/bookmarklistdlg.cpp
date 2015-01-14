#include <QKeyEvent>

#include "cr3widget.h"
#include "props.h"

#include "bookmarklistdlg.h"
#include "ui_bookmarklistdlg.h"

#define MAX_ITEM_LEN 50
static QString limit(QString s)
{
    if (s.length() < MAX_ITEM_LEN) return s;
    s = s.left(MAX_ITEM_LEN);
    return s + "...";
}

class BookmarkItem: public QTableWidgetItem
{
    CRBookmark* bm;
public:
    BookmarkItem( const QString& text, CRBookmark* b )
        : QTableWidgetItem(text)
        , bm(b)
    {}
    CRBookmark* getBookmark() const
    { return bm; }
};

BookmarkListDialog::BookmarkListDialog(QWidget *parent, CR3View * docView )
    : Dialog(parent)
    , ui(new Ui::BookmarkListDialog)
    , docview(docView)
    , pList(NULL)
{
    ui->setupUi(this);

    QFont f(ui->lblTitle->font());
    f.setBold(true);
    ui->lblTitle->setFont(f);

    f = ui->lblPosition->font();
    f.setPointSize(f.pointSize()/2);
    ui->lblPosition->setFont(f);

    ui->tableWidget->horizontalHeader()->setResizeMode( 0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setResizeMode( 1, QHeaderView::Stretch );
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    bool isVscroll = docview->getOptions()->getIntDef(PROP_APP_UI_BOOKMARKS_VSCROLLBAR, 1) != 0;
    ui->tableWidget->setVerticalScrollBarPolicy(isVscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    bool isHscroll = docview->getOptions()->getIntDef(PROP_APP_UI_BOOKMARKS_HSCROLLBAR, 1) != 0;
    ui->tableWidget->setHorizontalScrollBarPolicy(isHscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);

    CRFileHistRecord* rec = docview->getDocView()->getCurrentFileHistRecord();

    if ( !rec ) return;

    pList = &rec->getBookmarks();

    QSize s(ui->btnOk->iconSize());
    QPixmap pm(QPixmap(":/icons/book.png").scaled(s.width(), s.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    int y = 0;
    for ( int i=0; i<pList->length(); i++, y++ )
    {
        CRBookmark* bm = (*pList)[i];
        if ( bm->getType() != bmkt_lastpos )
        {
            ui->tableWidget->setRowCount(y+1);
            BookmarkItem* pbi = new BookmarkItem(crpercent( bm->getPercent()), bm);
            pbi->setData(Qt::DecorationRole, pm);
            ui->tableWidget->setItem( y, 0, pbi);
            ui->tableWidget->setItem( y, 1, new QTableWidgetItem(limit(cr2qt(bm->getPosText()))));
            ui->tableWidget->verticalHeader()->setResizeMode(y, QHeaderView::ResizeToContents);
        }
    }

    ui->tableWidget->setCurrentCell(0,0);
    ui->tableWidget->setEditFocus(true);

    QObject::connect( ui->tableWidget, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(onClicked(QTableWidgetItem*)) );
    QObject::connect( ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onClicked(QTableWidgetItem*)) );

    ui->navBar->setControls(ui->tableWidget, ui->lblPosition,
                            QSize( docview->getOptions()->getIntDef(PROP_APP_UI_SWIPES_X_WEIGHT, 1),
                                   docview->getOptions()->getIntDef(PROP_APP_UI_SWIPES_Y_WEIGHT, 1) ) );

    init();
}

BookmarkListDialog::~BookmarkListDialog()
{
    delete ui;
}

CRBookmark* BookmarkListDialog::selectedBookmark()
{
    int index = ui->tableWidget->currentRow();
    if ( index < 0 || index >= ui->tableWidget->rowCount() )
        return NULL;
    return static_cast<BookmarkItem*>(ui->tableWidget->item(index, 0))->getBookmark();
}

void BookmarkListDialog::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Qt::Key_Menu:
        if ( ui->tableWidget->hasFocus() )
            ui->btnDelete->setFocus();
        else
            ui->tableWidget->setEditFocus(true);
        break;
    default:
        break;
    }
}

void BookmarkListDialog::accept()
{
    CRBookmark* bm = selectedBookmark();
    if ( bm )
    {
        docview->goToBookmark(bm);
        QDialog::accept();
    }
}

void BookmarkListDialog::on_btnDelete_clicked()
{
    int index = ui->tableWidget->currentRow();
    if ( index >= 0 && index < ui->tableWidget->rowCount() )
    {
        CRBookmark* bm = selectedBookmark();
        if ( bm )
        {
            ui->tableWidget->removeRow(index);
            pList->remove(bm);
        }
    }
}

void BookmarkListDialog::onClicked(QTableWidgetItem *pItem)
{
    Q_UNUSED(pItem)
    accept();
}
