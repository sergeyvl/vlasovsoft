#include <QLineEdit>
#include <QAbstractTableModel>

#include "Platform.h"

#include "props.h"
#include "settings.h"
#include "ui_settings.h"

static bool initDone = false;

#define ZONES_SIZE 3

class TapZonesTableModel: public QAbstractTableModel
{
    int mode; // 0-short tap; 1=long tap
    int actions[2][ZONES_SIZE][ZONES_SIZE];

public:
    TapZonesTableModel( QObject* parent = 0 )
        : QAbstractTableModel(parent)
        , mode(0)
    {
        for ( int z=0; z<2; ++z )
            for ( int y=0; y<ZONES_SIZE; ++y)
                for (int x=0; x<ZONES_SIZE; ++x)
                    actions[z][y][x] = 0;
    }

    int getMode() const
    { return mode; }

    void setMode( int value )
    {
        mode = value;
        emit dataChanged(index(0,0),index(2,2));
    }

    int getData( int t, int y, int x ) const
    {
        return actions[t][y][x];
    }

    void setData( int t, int y, int x, int value )
    {
        actions[t][y][x] = value;
        emit dataChanged(index(y,x),index(y,x));
    }

    int rowCount ( const QModelIndex& parent = QModelIndex() ) const
    {
        Q_UNUSED(parent)
        return ZONES_SIZE;
    }
    int columnCount ( const QModelIndex& parent = QModelIndex() ) const
    {
        Q_UNUSED(parent)
        return ZONES_SIZE;
    }
    QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const
    {
        if ( Qt::DisplayRole == role )
        {
            int action = getData(mode, index.row(), index.column());
            return action > 0? QVariant(action):QVariant();
        }
        else
        if ( Qt::TextAlignmentRole == role )
            return Qt::AlignHCenter + Qt::AlignVCenter;

        return QVariant();
    }
};

SettingsDlg::SettingsDlg(QWidget* parent, CR3View* docView, const ActionList& aList )
    : Dialog(parent)
    , ui(new Ui::SettingsDlg)
    , docview( docView )
    , aList_(aList)
{
    initDone = false;

    ui->setupUi(this);

    addAction(ui->actionSaveSettings);

    props = docview->getOptions();
    optionToUi(PROP_FOOTNOTES, ui->ShowFootNotes);
    optionToUi(PROP_SHOW_BATTERY, ui->ShowBattery);
    optionToUi(PROP_SHOW_BATTERY_PERCENT, ui->ShowBatteryInPercent);
    optionToUi(PROP_SHOW_TIME, ui->ShowClock);
    optionToUi(PROP_SHOW_TITLE, ui->ShowBookName);
    optionToUi(PROP_TXT_OPTION_PREFORMATTED, ui->cbTxtPreFormatted);
    optionToUi(PROP_EMBEDDED_STYLES, ui->cbEnableDocumentStyles);
    optionToUi(PROP_EMBEDDED_FONTS, ui->cbEnableEmbeddedFonts);
    ui->cbEnableEmbeddedFonts->setEnabled(props->getIntDef(PROP_EMBEDDED_STYLES, 1) == 1 ? Qt::Checked : Qt::Unchecked);
    optionToUi(PROP_FLOATING_PUNCTUATION, ui->FloatingPunctuation);
    optionToUi(PROP_STATUS_CHAPTER_MARKS, ui->ChapterMarks);
    optionToUi(PROP_SHOW_POS_PERCENT, ui->PositionPercent);
    optionToUi(PROP_FONT_WEIGHT_EMBOLDEN, ui->cbEmbolden);
    optionToUi(PROP_FONT_KERNING_ENABLED, ui->cbKerning);

    int state1 = props->getIntDef(PROP_SHOW_PAGE_NUMBER, 1);
    int state2 = props->getIntDef(PROP_SHOW_PAGE_COUNT, 1);
    ui->PageNumber->setCheckState(state1==1 || state2==1? Qt::Checked : Qt::Unchecked);

    optionToUiInversed(PROP_STATUS_LINE, ui->ShowPageHeader);
    bool checked = !(props->getIntDef(PROP_STATUS_LINE, 1)==1);
    enableHeader(checked);

    ui->cbStartupAction->setCurrentIndex(props->getIntDef(PROP_APP_START_ACTION, 0 ));

    int lp = props->getIntDef(PROP_LANDSCAPE_PAGES, 2);
    ui->cbViewMode->setCurrentIndex(lp==1 ? 0 : 1);

    ui->sbMarginTop->setValue( props->getIntDef(PROP_PAGE_MARGIN_TOP, 8) );
    ui->sbMarginBottom->setValue( props->getIntDef(PROP_PAGE_MARGIN_BOTTOM, 8) );
    ui->sbMarginLeft->setValue( props->getIntDef(PROP_PAGE_MARGIN_LEFT, 8) );
    ui->sbMarginRight->setValue( props->getIntDef(PROP_PAGE_MARGIN_RIGHT, 8) );

    QStringList faceList;
    crGetFontFaceList(faceList);

    // fonts that are known to cause problems
    QStringList blackListFonts = QStringList() << "HYGothic" << "HYMyeongJo" << "MYing Hei T" << "Song T";
    QString toRemove;
    foreach (toRemove, blackListFonts) {
        faceList.removeAll(toRemove);
    }

    ui->cbTextFontFace->addItems(faceList);
    ui->cbTitleFontFace->addItems(faceList);

    ui->sbTextFontSize->setMinimum(MIN_CR_FONT_SIZE);
    ui->sbTextFontSize->setMaximum(MAX_CR_FONT_SIZE);
    ui->sbTitleFontSize->setMinimum(MIN_CR_HEADER_FONT_SIZE);
    ui->sbTitleFontSize->setMaximum(MAX_CR_HEADER_FONT_SIZE);

    const char * defFontFace = "Arial";
    static const char * goodFonts[] = { "Arial", "Verdana", NULL };
    for(int i=0; goodFonts[i]; i++) {
        if(faceList.indexOf(QString(goodFonts[i]))>=0) {
            defFontFace = goodFonts[i];
            break;
        }
    }

    fontToUi(PROP_FONT_FACE, PROP_FONT_SIZE, ui->cbTextFontFace, ui->sbTextFontSize, defFontFace);
    fontToUi(PROP_STATUS_FONT_FACE, PROP_STATUS_FONT_SIZE, ui->cbTitleFontFace, ui->sbTitleFontSize, defFontFace);

    ui->sbInterlineSpace->setValue(props->getIntDef(PROP_INTERLINE_SPACE, 100));

    ui->sbSpaceCond->setValue(props->getIntDef(PROP_FORMAT_MIN_SPACE_CONDENSING_PERCENT, DEF_MIN_SPACE_CONDENSING_PERCENT));
    ui->cbAA->setCurrentIndex(props->getIntDef(PROP_FONT_ANTIALIASING, font_aa_all));
    ui->cbHinting->setCurrentIndex(props->getIntDef(PROP_FONT_HINTING, HINTING_MODE_AUTOHINT));

    int updateEvery = props->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1);
    int upIndex = ui->cbFullUpdateEvery->findText(QString::number(updateEvery));
    ui->cbFullUpdateEvery->setCurrentIndex(upIndex != -1 ? upIndex : 1);

    // translations
    QString v = props->getStringDef(PROP_APP_UI_LANG, "English");
    QDir Dir(Platform::get()->getLocalTranslationsPath());
    QStringList filter;
    filter << "*.qm";
    QStringList files = Dir.entryList(filter, QDir::Files, QDir::Name);
    ui->cbLanguage->addItem("English");
    foreach(const QString &str, files)
    {
        if (str.contains("_")) continue; // ignore qt_ translations
        QString s = str;
        s.chop(3);
        ui->cbLanguage->addItem(s);
    }

    int ind = ui->cbLanguage->findText(v);
    ui->cbLanguage->setCurrentIndex(ind != -1 ? ind : 1);

    int hi = -1;
    v = props->getStringDef(PROP_HYPHENATION_DICT,"@algorithm");
    for ( int i=0; i<HyphMan::getDictList()->length(); i++ ) 
    {
        HyphDictionary * item = HyphMan::getDictList()->get( i );
        if (v == cr2qt(item->getFilename() ) ||  v == cr2qt(item->getId() ))
            hi = i;
        QString s = cr2qt( item->getTitle() );
        if( item->getType()==HDT_NONE)
            s = tr("Off");
        else if( item->getType()==HDT_ALGORITHM)
            s = tr("Algorythmic");
        ui->cbHyphenation->addItem(s);
    }
    ui->cbHyphenation->setCurrentIndex(hi>=0 ? hi : 1);

    double fgamma = props->getDoubleDef(PROP_FONT_GAMMA, "1");
    ui->sbFontGamma->setValue(fgamma);

    lString16 testPhrase = qt2cr(tr("The quick brown fox jumps over the lazy dog. "));
    ui->crSample->getDocView()->createDefaultDocument(lString16::empty_str, testPhrase+testPhrase+testPhrase+testPhrase+testPhrase+testPhrase);

    ui->chkInverse->setChecked( 1 == props->getIntDef(PROP_DISPLAY_INVERSE, 2) );

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidgetStyles->setCurrentIndex(0);
    ui->tabWidgetPage->setCurrentIndex(0);

    // Fill tap zones table model
    TapZonesTableModel* tzModel = new TapZonesTableModel();
    for ( int t=0; t<2; ++t )
        for ( int y=0; y<tzModel->rowCount(); ++y )
            for ( int x=0; x<tzModel->columnCount(); ++x)
            {
                tzModel->setData(t, y, x, props->getIntDef(aList_.getTapZonesParamName(t,x,y).toAscii().constData(), 0));
            }

    ui->tapZonesTableView->setModel(tzModel);
    ui->tapZonesTableView->horizontalHeader()->hide();
    ui->tapZonesTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tapZonesTableView->verticalHeader()->hide();
    for ( int i=0; i<ZONES_SIZE; ++i )
    {
        ui->tapZonesTableView->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
        ui->tapZonesTableView->verticalHeader()->setResizeMode(i, QHeaderView::Stretch);
    }
    QObject::connect( ui->tapZonesTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                      this, SLOT(onTapZonesTableViewSelectionChanged(QItemSelection,QItemSelection)));

    // Fill actions combobox
    int n = 0;
    foreach (const QAction* a, aList)
    {
        QString act(QString("%1 - %2").arg(n++).arg(a->text()));
        ui->cbTapZonesAction->addItem(act);
        ui->cbSwipesAction->addItem(act);
        ui->cbKeysAction->addItem(act);
    }

    updateStyleSample();

    init();

    initDone = true;

    on_cbSwipesEvent_currentIndexChanged( ui->cbSwipesEvent->currentIndex() );
    on_cbKeyValue_currentIndexChanged( ui->cbKeyValue->currentText() );
}

SettingsDlg::~SettingsDlg()
{
    delete ui;
}

void SettingsDlg::optionToUi( const char * optionName, QCheckBox * cb )
{
    int state = props->getIntDef(optionName, 1);
    cb->setCheckState(state==1 ? Qt::Checked : Qt::Unchecked);
}

void SettingsDlg::optionToUiInversed( const char * optionName, QCheckBox * cb )
{
    int state = props->getIntDef(optionName, 1);
    cb->setCheckState(state==1 ? Qt::Unchecked : Qt::Checked);
}

void SettingsDlg::setCheck(const char * optionName, bool checkState) {
    props->setInt(optionName, checkState);
}

void SettingsDlg::on_cbViewMode_currentIndexChanged(int index)
{
    if(!initDone) return;
    props->setInt(PROP_LANDSCAPE_PAGES, index + 1);
    props->setInt(PROP_PAGE_VIEW_MODE, 1);
}

void SettingsDlg::on_ShowPageHeader_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_STATUS_LINE, !checked);
    enableHeader(checked);
    updateStyleSample();
}

void SettingsDlg::on_ShowBookName_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_TITLE, checked);
    updateStyleSample();
}

void SettingsDlg::on_ShowClock_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_TIME, checked);
    updateStyleSample();
}

void SettingsDlg::on_ShowBattery_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_BATTERY, checked);
    updateStyleSample();
}

void SettingsDlg::on_ShowBatteryInPercent_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_BATTERY_PERCENT, checked);
    updateStyleSample();
}

void SettingsDlg::on_ShowFootNotes_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FOOTNOTES, checked);
    updateStyleSample();
}

void SettingsDlg::updateStyleSample()
{
    ui->crSample->propsApply(props);
    LVDocView* dv = ui->crSample->getDocView();
    dv->setShowCover(false);
    dv->getPageImage(0);
}

void SettingsDlg::on_cbTitleFontFace_currentIndexChanged(QString s)
{
    if(!initDone) return;
    props->setString(PROP_STATUS_FONT_FACE, s);
}

void SettingsDlg::on_cbTextFontFace_currentIndexChanged(QString s)
{
    if(!initDone) return;
    props->setString(PROP_FONT_FACE, s);
    updateStyleSample();
}

void SettingsDlg::fontToUi( const char * faceOptionName, const char * sizeOptionName, QComboBox * faceCombo, QSpinBox * sizeSpinBox, const char * defFontFace )
{
    QString faceName = props->getStringDef(faceOptionName, defFontFace);
    int faceIndex = faceCombo->findText(faceName);
    if(faceIndex>=0) faceCombo->setCurrentIndex(faceIndex);

    int size;
    props->getInt(sizeOptionName, size);
    sizeSpinBox->setValue(size);
}

void SettingsDlg::on_cbHyphenation_currentIndexChanged(int index)
{
    if(!initDone) return;
    const QStringList & dl(docview->getHyphDicts());
    QString s = dl[index < dl.count() ? index : 1];
    props->setString(PROP_HYPHENATION_DICT, s);
}

void SettingsDlg::on_cbStartupAction_currentIndexChanged(int index)
{
    if(!initDone) return;
    props->setInt(PROP_APP_START_ACTION, index);
}

void SettingsDlg::on_cbLanguage_currentIndexChanged(const QString &arg1)
{
    if(!initDone) return;
    props->setString(PROP_APP_UI_LANG, arg1);
}

void SettingsDlg::on_sbFontGamma_valueChanged(double arg1)
{
    if(!initDone) return;
    props->setString(PROP_FONT_GAMMA, QString::number(arg1));
    updateStyleSample();
}

void SettingsDlg::on_actionSaveSettings_triggered()
{   
    accept();
}

void SettingsDlg::on_actionNextTab_triggered()
{
    int CurrInd = ui->tabWidget->currentIndex();
    if(CurrInd+1 == ui->tabWidget->count())
        ui->tabWidget->setCurrentIndex(0);
    else
        ui->tabWidget->setCurrentIndex(CurrInd+1);
}

void SettingsDlg::on_actionPrevTab_triggered()
{
    int CurrInd = ui->tabWidget->currentIndex();
    if(CurrInd == 0)
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    else
        ui->tabWidget->setCurrentIndex(CurrInd-1);
}

void SettingsDlg::on_sbTextFontSize_valueChanged(int arg1)
{
    if(!initDone) return;
    props->setInt(PROP_FONT_SIZE, arg1);
    updateStyleSample();
}

void SettingsDlg::on_sbTitleFontSize_valueChanged(int arg1)
{
    if(!initDone) return;
    props->setInt(PROP_STATUS_FONT_SIZE, arg1);
    updateStyleSample();
}

void SettingsDlg::on_sbInterlineSpace_valueChanged(int arg1)
{
    if(!initDone) return;
    props->setInt(PROP_INTERLINE_SPACE, arg1);
    updateStyleSample();
}

void SettingsDlg::on_cbEmbolden_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FONT_WEIGHT_EMBOLDEN, checked);
    updateStyleSample();
}

void SettingsDlg::on_cbKerning_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FONT_KERNING_ENABLED, checked);
    updateStyleSample();
}

void SettingsDlg::on_cbTxtPreFormatted_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_TXT_OPTION_PREFORMATTED, checked);
}

void SettingsDlg::on_cbEnableEmbeddedFonts_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_EMBEDDED_FONTS, checked ? Qt::Checked : Qt::Unchecked);
}

void SettingsDlg::on_cbEnableDocumentStyles_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_EMBEDDED_STYLES, checked ? Qt::Checked : Qt::Unchecked);
    ui->cbEnableEmbeddedFonts->setEnabled(checked);
}

void SettingsDlg::on_FloatingPunctuation_toggled(bool checked)
{
    if (!initDone) return;
    setCheck(PROP_FLOATING_PUNCTUATION, checked);
    updateStyleSample();
}

void SettingsDlg::on_ChapterMarks_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_STATUS_CHAPTER_MARKS, checked);
    updateStyleSample();
}

void SettingsDlg::on_PositionPercent_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_POS_PERCENT, checked);
    updateStyleSample();
}

void SettingsDlg::on_PageNumber_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_PAGE_NUMBER, checked);
    setCheck(PROP_SHOW_PAGE_COUNT, checked);
    updateStyleSample();
}

void SettingsDlg::enableHeader(bool state)
{
    ui->gbHeaderFont->setEnabled(state);
    ui->gbHeaderElements->setEnabled(state);
}

void SettingsDlg::accept()
{
    docview->setOptions(props);
    Dialog::accept();
}

#define DEF_MARGIN	8

void SettingsDlg::onMarginChanged(int arg1)
{
    if(!initDone) return;

    QSpinBox* pSpb = qobject_cast<QSpinBox*>(sender());

    if ( !pSpb ) return;

    const char* propName = NULL;

    if ( pSpb == ui->sbMarginTop )
        propName = PROP_PAGE_MARGIN_TOP;
    else
    if ( pSpb == ui->sbMarginBottom )
        propName = PROP_PAGE_MARGIN_BOTTOM;
    else
    if ( pSpb == ui->sbMarginLeft )
        propName = PROP_PAGE_MARGIN_LEFT;
    else
    if ( pSpb == ui->sbMarginRight )
        propName = PROP_PAGE_MARGIN_RIGHT;
    else
        return;

    props->setInt(propName, arg1);

    updateStyleSample();
}

void SettingsDlg::on_cbFullUpdateEvery_currentIndexChanged(const QString &arg1)
{
    if(!initDone) return;
    props->setInt(PROP_DISPLAY_FULL_UPDATE_INTERVAL, arg1.toInt());
}

void SettingsDlg::on_cbAA_currentIndexChanged(int index)
{
    if(!initDone) return;
    props->setInt(PROP_FONT_ANTIALIASING, index);
    updateStyleSample();
}

void SettingsDlg::on_cbHinting_currentIndexChanged(int index)
{
    if(!initDone) return;
    props->setInt(PROP_FONT_HINTING, index);
    updateStyleSample();
}

void SettingsDlg::on_sbSpaceCond_valueChanged(int arg1)
{
    if(!initDone) return;
    props->setInt(PROP_FORMAT_MIN_SPACE_CONDENSING_PERCENT, arg1);
    updateStyleSample();
}

void SettingsDlg::on_chkInverse_toggled(bool checked)
{
    if(!initDone) return;
    props->setInt(PROP_DISPLAY_INVERSE, checked? 1:2);
    updateStyleSample();
}

void SettingsDlg::onTapZonesTableViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected)
    TapZonesTableModel* model = static_cast<TapZonesTableModel*>(ui->tapZonesTableView->model());
    QModelIndexList indexes(selected.indexes());
    if ( !indexes.isEmpty() )
    {
        const QModelIndex& index = indexes.at(0);
        int idx = model->getData(model->getMode(), index.row(), index.column());
        initDone = false;
        ui->cbTapZonesAction->setCurrentIndex(idx);
        initDone = true;
    }
}

void SettingsDlg::on_cbTapZonesAction_currentIndexChanged(int index)
{
    if ( initDone )
    {
        TapZonesTableModel* model = static_cast<TapZonesTableModel*>(ui->tapZonesTableView->model());
        const QItemSelection& selection = ui->tapZonesTableView->selectionModel()->selection();
        QModelIndexList indexes(selection.indexes());
        if ( !indexes.empty() )
        {
            const QModelIndex& idx = indexes.at(0);
            model->setData(model->getMode(), idx.row(), idx.column(), index);
            props->setInt(aList_.getTapZonesParamName(model->getMode(), idx.column(), idx.row()).toAscii().constData(), model->getData(model->getMode(), idx.row(), idx.column()));
        }
    }
}

void SettingsDlg::on_cbTapZonesEvent_currentIndexChanged(int index)
{
    if ( initDone )
    {
        TapZonesTableModel* model = static_cast<TapZonesTableModel*>(ui->tapZonesTableView->model());
        model->setMode( index );
        const QItemSelection& selection = ui->tapZonesTableView->selectionModel()->selection();
        QModelIndexList indexes(selection.indexes());
        if ( !indexes.empty() )
        {
            const QModelIndex& idx = indexes.at(0);
            initDone = false;
            ui->cbTapZonesAction->setCurrentIndex( model->getData( model->getMode(), idx.row(), idx.column() ) );
            initDone = true;
        }
    }
}

void SettingsDlg::on_cbSwipesEvent_currentIndexChanged(int index)
{
    int i = props->getIntDef( aList_.getSwipesParamName(index).toAscii().constData(), 0 );
    initDone = false;
    ui->cbSwipesAction->setCurrentIndex(i);
    initDone = true;
}

void SettingsDlg::on_cbSwipesAction_currentIndexChanged(int index)
{
    if ( initDone )
    {
        props->setInt(aList_.getSwipesParamName(ui->cbSwipesEvent->currentIndex()).toAscii().constData(), index);
    }
}

void SettingsDlg::on_cbKeysAction_currentIndexChanged(int index)
{
    if ( initDone )
    {
        QString propName(QString(PROP_APP_UI_KEY_MAPPING).arg(ui->cbKeyValue->currentText()));
        props->setInt(propName.toAscii().constData(), index);
    }
}

void SettingsDlg::on_cbKeyValue_currentIndexChanged(const QString &arg1)
{
    if ( initDone )
    {
        QString propName(QString(PROP_APP_UI_KEY_MAPPING).arg(arg1));
        initDone = false;
        ui->cbKeysAction->setCurrentIndex( props->getIntDef( propName.toAscii().constData(), 0 ) );
        initDone = true;
    }
}
