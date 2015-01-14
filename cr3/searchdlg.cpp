#include "QtUtils.h"
#include "searchdlg.h"
#include "ui_searchdlg.h"

#include <QTimer>
#include <QKeyEvent>
#include <QSettings>

bool SearchDialog::reverse = false;
bool SearchDialog::caseSensitive = false;
QString SearchDialog::lastText;

SearchDialog::SearchDialog(QWidget* parent)
    : Dialog(parent)
    , ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    initTopLevelWidget(this);

    QFont f(ui->lblTitle->font());
    f.setBold(true);
    ui->lblTitle->setFont(f);

    ui->cbCaseSensitive->setChecked(caseSensitive);
    if ( reverse )
        ui->rbBackward->setChecked(true);
    else
        ui->rbForward->setChecked(true);

    ui->edPattern->setFocus();

    if ( !lastText.isEmpty() )
    {
        QTimer::singleShot(0, this, SLOT(onSelectText()));
    }

    showFullScreen();
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::on_btnFind_clicked()
{
    text    = ui->edPattern->text();
    reverse = ui->rbBackward->isChecked();
    caseSensitive = ui->cbCaseSensitive->isChecked();
    accept();
}

void SearchDialog::onSelectText()
{
    ui->edPattern->setEditFocus(true);
    ui->edPattern->setText(lastText);
    ui->edPattern->selectAll();
}
