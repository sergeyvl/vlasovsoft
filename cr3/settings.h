#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QWSEvent>
#include <QColorDialog>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QKeyEvent>
#include <QApplication>
#include <QStyledItemDelegate>
#include <QItemSelection>

#include "Dialog.h"
#include "QtUtils.h"

#include "crqtutil.h"
#include "cr3widget.h"
#include "actionlist.h"
#include "keymapper.h"

namespace Ui {
class SettingsDlg;
}

class CR3View;

class SettingsDlg : public Dialog
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingsDlg)

public:
    SettingsDlg(QWidget* parent, CR3View* docView, const ActionList& aList );
    virtual ~SettingsDlg();

protected:
    void setCheck(const char * optionName, bool checkState);
    void optionToUi( const char * optionName, QCheckBox * cb );
    void setCheckInversed( const char * optionName, int checkState );
    void optionToUiInversed( const char * optionName, QCheckBox * cb );
    void fontToUi( const char * faceOptionName, const char * sizeOptionName, QComboBox * faceCombo, QSpinBox * sizeSpinBox, const char * defFontFace );
    void updateStyleSample();

private:
    Ui::SettingsDlg* ui;
    CR3View* docview;
    PropsRef props;
    KeyMapper kmap;
    const ActionList& aList_;

    void enableHeader(bool state);

private slots:
    virtual void accept();

    void on_cbStartupAction_currentIndexChanged(int index);
    void on_cbHyphenation_currentIndexChanged(int index);
    void on_cbTextFontFace_currentIndexChanged(QString);
    void on_cbTitleFontFace_currentIndexChanged(QString);
    void on_cbViewMode_currentIndexChanged(int index);
    void on_cbLanguage_currentIndexChanged(const QString &arg1);

    void on_ShowFootNotes_toggled(bool checked);
    void on_ShowBattery_toggled(bool checked);
    void on_ShowBatteryInPercent_toggled(bool checked);
    void on_ShowClock_toggled(bool checked);
    void on_ShowBookName_toggled(bool checked);
    void on_ShowPageHeader_toggled(bool checked);
    void on_cbTxtPreFormatted_toggled(bool checked);
    void on_cbEnableEmbeddedFonts_toggled(bool checked);
    void on_cbEnableDocumentStyles_toggled(bool checked);
    void on_FloatingPunctuation_toggled(bool checked);
    void on_ChapterMarks_toggled(bool checked);
    void on_PositionPercent_toggled(bool checked);
    void on_PageNumber_toggled(bool checked);

    void on_sbFontGamma_valueChanged(double arg1);
    void on_sbTextFontSize_valueChanged(int arg1);
    void on_sbTitleFontSize_valueChanged(int arg1);
    void on_sbInterlineSpace_valueChanged(int arg1);

    void on_actionSaveSettings_triggered();
    void on_actionNextTab_triggered();
    void on_actionPrevTab_triggered();
    void onMarginChanged(int arg1);
    void on_cbFullUpdateEvery_currentIndexChanged(const QString &arg1);
    void on_cbEmbolden_toggled(bool checked);
    void on_cbKerning_toggled(bool checked);
    void on_cbAA_currentIndexChanged(int index);
    void on_cbHinting_currentIndexChanged(int index);
    void on_sbSpaceCond_valueChanged(int arg1);
    void on_chkInverse_toggled(bool checked);
    void onTapZonesTableViewSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void on_cbTapZonesAction_currentIndexChanged(int index);
    void on_cbTapZonesEvent_currentIndexChanged(int index);
    void on_cbSwipesEvent_currentIndexChanged(int index);
    void on_cbSwipesAction_currentIndexChanged(int index);
    void on_cbKeysAction_currentIndexChanged(int index);
    void on_cbKeyValue_currentIndexChanged(const QString &arg1);
};

#endif // SETTINGSDLG_H
