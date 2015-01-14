#ifndef OPENFILEDLG_H
#define OPENFILEDLG_H

#include <QFileSystemModel>
#include <QFileIconProvider>

#include "Dialog.h"
#include "lvstream.h"
#include "cr3widget.h"
#include "crqtutil.h"
#include "settings.h"
#include "gesturescontroller.h"

namespace Ui {
class OpenFileDlg;
}

class CR3View;

class OpenFileDlg : public Dialog, public QFileIconProvider
{
    Q_OBJECT

public:
    OpenFileDlg(QWidget* parent, CR3View* docView, const QString& path = QString(), const QString& file = QString());
    virtual ~OpenFileDlg();
    
    // QFileIconProvider
    virtual QIcon	icon ( IconType type ) const;
    virtual QIcon	icon ( const QFileInfo & info ) const;
    virtual QString	type ( const QFileInfo & info ) const;

private:
    QFileSystemModel model;
    Ui::OpenFileDlg* ui;
    QIcon iconFolder, iconFile;
    QString prevPath_;

private:
    virtual void keyPressEvent( QKeyEvent* e );

private slots:
    void onClicked( const QModelIndex& index );
    void on_btnUp_clicked();
    void onLayoutChanged();

public slots:
    virtual void accept();

public:
    QString fileName;
};

#endif // OPENFILEDLG_H
