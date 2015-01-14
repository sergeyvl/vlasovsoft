#include <cmath>

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "props.h"

#include "tocdlg.h"
#include "ui_tocdlg.h"

class TocItem : public QTreeWidgetItem
{
    LVTocItem * _item;
public:
    LVTocItem* getItem() { return _item; }
    TocItem(LVTocItem * item, int currPage, int & nearestPage, TocItem*& nearestItem)
        : QTreeWidgetItem(QStringList() << (item ? cr2qt(item->getName()) : "No TOC items")
                                        << (item ? cr2qt(lString16::itoa(item->getPage()+1)) : ""))
        , _item(item)
    {
        setTextAlignment(1, Qt::AlignRight|Qt::AlignVCenter);
        int page = item->getPage();
        if (!nearestItem || (page <= currPage && page > nearestPage)) {
            nearestItem = this;
            nearestPage = page;
        }
        setData(0, Qt::UserRole, QVariant(cr2qt(item->getXPointer().toString())));
        for (int i = 0; i < item->getChildCount(); i++) {
            addChild(new TocItem(item->getChild(i), currPage, nearestPage, nearestItem));
        }
    }
};

TocDlg::TocDlg(QWidget *parent, CR3View * docView)
    : Dialog(parent)
    , ui(new Ui::TocDlg)
    , docview(docView)
{
    ui->setupUi(this);

    QFont f(ui->treeWidget->font());
    f.setPointSize(docView->getOptions()->getIntDef(PROP_APP_UI_TOC_FONT_SIZE, 12));
    ui->treeWidget->setFont(f);

    f = ui->lblTitle->font();
    f.setBold(true);
    ui->lblTitle->setFont(f);

    f = ui->lblPosition->font();
    f.setPointSize(f.pointSize()/2);
    ui->lblPosition->setFont(f);

    ui->treeWidget->setColumnCount(2);

    bool isVscroll = docview->getOptions()->getIntDef(PROP_APP_UI_TOC_VSCROLLBAR, 1) != 0;
    ui->treeWidget->setVerticalScrollBarPolicy(isVscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    bool isHscroll = docview->getOptions()->getIntDef(PROP_APP_UI_TOC_HSCROLLBAR, 1) != 0;
    ui->treeWidget->setHorizontalScrollBarPolicy(isHscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);

    ui->treeWidget->header()->setDefaultAlignment(Qt::AlignLeft);
    ui->treeWidget->header()->setResizeMode(0, isHscroll? QHeaderView::ResizeToContents : QHeaderView::Stretch);
    ui->treeWidget->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->setExpandsOnDoubleClick(false);
    ui->treeWidget->setEditFocus(true);
  
    int nearestPage = -1;
    int currPage = docView->getCurPage();
    TocItem * nearestItem = NULL;
    LVTocItem * root = docview->getToc();
    for (int i=0; i<root->getChildCount(); i++ )
        ui->treeWidget->addTopLevelItem(new TocItem(root->getChild(i), currPage, nearestPage, nearestItem));

    ui->treeWidget->expandAll();
    if (nearestItem)
    {
        ui->treeWidget->setCurrentItem(nearestItem);
        ui->treeWidget->scrollToItem(nearestItem, QAbstractItemView::EnsureVisible);
    }

    QObject::connect( ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onClicked(QTreeWidgetItem*,int)) );
    QObject::connect( ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onClicked(QTreeWidgetItem*,int)) );

    ui->navBar->setControls(ui->treeWidget, ui->lblPosition,
                              QSize( docview->getOptions()->getIntDef(PROP_APP_UI_SWIPES_X_WEIGHT, 1),
                                     docview->getOptions()->getIntDef(PROP_APP_UI_SWIPES_Y_WEIGHT, 1) ) );

    init();
}

TocDlg::~TocDlg()
{
    delete ui;
}

void TocDlg::accept()
{
    TocItem* pItem = static_cast<TocItem*>(ui->treeWidget->currentItem());
    if ( pItem )
    {
        int page = pItem->getItem()->getPage();
        docview->GoToPage(page);
        QDialog::accept();
    }
}

void TocDlg::onClicked(QTreeWidgetItem *pItem, int)
{
    if ( pItem )
    {
        if ( 0 == pItem->childCount() )
        {
            TocItem* pTocItem = static_cast<TocItem*>(pItem);
            int page = pTocItem->getItem()->getPage();
            docview->GoToPage(page);
            QDialog::accept();
        }
        else
        {
            if ( pItem->isExpanded() )
                ui->treeWidget->collapseItem( pItem );
            else
                ui->treeWidget->expandItem( pItem );
        }
    }
}


void TocDlg::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {
    case Qt::Key_Menu:
        if ( ui->treeWidget->hasFocus() )
            ui->btnCancel->setFocus();
        else
            ui->treeWidget->setEditFocus(true);
        break;
    default:
        break;
    }
}
