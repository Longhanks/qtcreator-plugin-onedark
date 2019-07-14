#include "optionsdialog.h"

#include "settings.h"

#include <QBoxLayout>
#include <QCheckBox>

namespace OneDark::Internal {

OptionsDialog::OptionsDialog(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);

    auto checkBoxEnableTabBarThemeText =
        tr("Enable tab bar theme (restart required)");
    this->checkBoxEnableTabBarTheme =
        new QCheckBox(checkBoxEnableTabBarThemeText, this);
    layout->addWidget(this->checkBoxEnableTabBarTheme);

    auto checkBoxForceHideMnemonicsText =
        tr("Force hide mnemonics (restart required)");
    this->checkBoxForceHideMnemonics =
        new QCheckBox(checkBoxForceHideMnemonicsText, this);
    layout->addWidget(this->checkBoxForceHideMnemonics);

    auto checkBoxDisableDialogButtonBoxIconsText =
        tr("Disable dialog button box icons");
    this->checkBoxDisableDialogButtonBoxIcons =
        new QCheckBox(checkBoxDisableDialogButtonBoxIconsText, this);
    layout->addWidget(this->checkBoxDisableDialogButtonBoxIcons);

    auto checkBoxSuppressHighlightColorFocusedTreeViewItemsText =
        tr("Suppress special highlight color for focused tree view items");
    this->checkBoxSuppressHighlightColorFocusedTreeViewItems = new QCheckBox(
        checkBoxSuppressHighlightColorFocusedTreeViewItemsText, this);
    layout->addWidget(
        this->checkBoxSuppressHighlightColorFocusedTreeViewItems);

    layout->addStretch();
    this->setLayout(layout);
}

void OptionsDialog::setSettings(const Settings &settings) {
    this->checkBoxEnableTabBarTheme->setChecked(settings.enableTabBarTheme);
    this->checkBoxForceHideMnemonics->setChecked(settings.forceHideMnemonics);
    this->checkBoxDisableDialogButtonBoxIcons->setChecked(
        settings.disableDialogButtonBoxIcons);
    this->checkBoxSuppressHighlightColorFocusedTreeViewItems->setChecked(
        settings.suppressHighlightColorFocusedTreeViewItems);
}

Settings OptionsDialog::settings() {
    Settings settings;
    settings.enableTabBarTheme = this->checkBoxEnableTabBarTheme->isChecked();
    settings.forceHideMnemonics =
        this->checkBoxForceHideMnemonics->isChecked();
    settings.disableDialogButtonBoxIcons =
        this->checkBoxDisableDialogButtonBoxIcons->isChecked();
    settings.suppressHighlightColorFocusedTreeViewItems =
        this->checkBoxSuppressHighlightColorFocusedTreeViewItems->isChecked();
    return settings;
}

} // namespace OneDark::Internal
