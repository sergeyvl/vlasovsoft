#include "props.h"

#include "openfiledlg.h"
#include "ui_openfiledlg.h"

OpenFileDlg::OpenFileDlg(QWidget* parent, CR3View* docView, const QString& path, const QString& file)
    : Dialog(parent)
    , ui(new Ui::OpenFileDlg)
{
    ui->setupUi(this);

    QFont f(ui->fileList->font());
    f.setPointSize(docView->getOptions()->getIntDef(PROP_APP_UI_OPENFILE_FONT_SIZE, 12));
    ui->fileList->setFont(f);

    f = ui->lblPosition->font();
    f.setPointSize(f.pointSize()/2);
    ui->lblPosition->setFont(f);

    f = ui->lblTitle->font();
    f.setBold(true);
    ui->lblTitle->setFont(f);

    iconFile = QIcon(":/icons/book.png");
    iconFolder = QIcon(":/icons/folder.png");

    model.setIconProvider(this);

    model.setNameFilterDisables(false);
    model.setRootPath("/");
    ui->fileList->setModel( &model );
    ui->fileList->setRootIndex(model.index(path));
    if ( !file.isEmpty() )
    {
        ui->fileList->setCurrentIndex(model.index(path + QDir::separator() + file));
    }

    model.setFilter( QDir::AllDirs | QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden );
    model.setNameFilters( QStringList() << "*.zip" << "*.fb2" << "*.epub" << "*.rtf" << "*.txt" << "*.html" << "*.htm" << "*.tcr" << "*.pdb" << "*.chm" << "*.mobi" << "*.doc" << "*.azw" );

    bool isVscroll = docView->getOptions()->getIntDef(PROP_APP_UI_OPENFILE_VSCROLLBAR, 1) != 0;
    ui->fileList->setVerticalScrollBarPolicy(isVscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    bool isHscroll = docView->getOptions()->getIntDef(PROP_APP_UI_OPENFILE_HSCROLLBAR, 1) != 0;
    ui->fileList->setHorizontalScrollBarPolicy(isHscroll? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);

    QObject::connect( ui->fileList, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    QObject::connect( ui->fileList, SIGNAL(activated(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    QObject::connect( &model, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));

    ui->navBar->setControls(ui->fileList, ui->lblPosition,
                            QSize( docView->getOptions()->getIntDef(PROP_APP_UI_SWIPES_X_WEIGHT, 1),
                                   docView->getOptions()->getIntDef(PROP_APP_UI_SWIPES_Y_WEIGHT, 1) ) );

    ui->fileList->setEditFocus(true);

    int iconSize = 0;
    if (    docView->getOptions()->getInt( PROP_APP_UI_OPENFILE_ICON_SIZE, iconSize )
         && iconSize >= 10
         && iconSize <= 200 )
    {
        ui->fileList->setIconSize(QSize(iconSize,iconSize));
    }
    ui->fileList->setTextElideMode(Qt::ElideRight);

    init();
}

OpenFileDlg::~OpenFileDlg()
{
    delete ui;
}

QIcon OpenFileDlg::icon( IconType type ) const
{
    switch ( type )
    {
    case QFileIconProvider::Folder:
        return iconFolder;
        break;
    case QFileIconProvider::File:
        return iconFile;
        break;
    default:
        return QFileIconProvider::icon( type );
    }
}

QIcon OpenFileDlg::icon ( const QFileInfo& info ) const
{
    if ( info.isFile() )
        return iconFile;
    else
    if ( info.isDir() )
        return iconFolder;
    else
        return QFileIconProvider::icon( info );
}

QString	OpenFileDlg::type ( const QFileInfo & info ) const
{
    return QFileIconProvider::type( info );
}

void OpenFileDlg::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {
    case Qt::Key_Left:
        if ( ui->fileList->hasEditFocus() )
            on_btnUp_clicked();
        break;
    case Qt::Key_Menu:
        if ( ui->fileList->hasEditFocus() )
            ui->btnUp->setFocus();
        else
            ui->fileList->setEditFocus(true);
        break;
    default:
        break;
    }
}

void OpenFileDlg::onClicked( const QModelIndex& index )
{
    Q_UNUSED(index)
    accept();
}

void OpenFileDlg::accept()
{
    QModelIndex index( ui->fileList->currentIndex() );
    if ( index.isValid() )
    {
        if ( model.isDir( index ) )
        {
            ui->fileList->setRootIndex( index );
            onLayoutChanged();
        }
        else
        {
            fileName = model.filePath( index );
            QDialog::accept();
        }
    }
}

void OpenFileDlg::on_btnUp_clicked()
{
    QModelIndex oldRoot(ui->fileList->rootIndex());
    QModelIndex newRoot(model.parent(oldRoot));
    if ( newRoot.isValid() )
    {
        ui->fileList->setRootIndex( newRoot );
        ui->fileList->setCurrentIndex( oldRoot );
        onLayoutChanged();
    }
}

void OpenFileDlg::onLayoutChanged()
{
    if (    !ui->fileList->currentIndex().isValid()
         || ui->fileList->currentIndex() == ui->fileList->rootIndex() )
    {
        ui->fileList->setCurrentIndex(model.index(0,0,ui->fileList->rootIndex()));
    }
    ui->fileList->scrollTo(ui->fileList->currentIndex(), QAbstractItemView::PositionAtCenter);
}
