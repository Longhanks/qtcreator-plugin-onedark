#include "optionspage.h"

#include <QBoxLayout>
#include <QCheckBox>

namespace OneDark::Internal {

OptionsPage::OptionsPage(QObject *parent) : IOptionsPage(parent, true) {
    this->setId("OneDarkSettings");
    this->setDisplayName("General");
    this->setCategory("OneDark");
    this->setDisplayCategory("OneDark");
    auto iconMask = QStringLiteral(":/atom.png");
    auto iconColor = Utils::Theme::PanelTextColorDark;
    auto iconStyle = Utils::Icon::Tint;
    this->setCategoryIcon(Utils::Icon({{iconMask, iconColor}}, iconStyle));
}

QWidget *OptionsPage::widget() {
    if (!this->m_widget) {
        this->m_widget = new QWidget;
        auto layout = new QVBoxLayout(this->m_widget);

        auto checkBoxEnableTabBarThemeText = tr("Enable tab bar theme");
        auto checkBoxEnableTabBarTheme =
            new QCheckBox(checkBoxEnableTabBarThemeText, this->m_widget);
        layout->addWidget(checkBoxEnableTabBarTheme);

        auto checkBoxHideMnemonicsText =
            tr("Hide mnemonics unless Alt is pressed");
        auto checkBoxHideMnemonics =
            new QCheckBox(checkBoxHideMnemonicsText, this->m_widget);
        layout->addWidget(checkBoxHideMnemonics);

        auto checkBoxDisableDialogButtonBoxIconsText =
            tr("Disable dialog button box icons");
        auto checkBoxDisableDialogButtonBoxIcons = new QCheckBox(
            checkBoxDisableDialogButtonBoxIconsText, this->m_widget);
        layout->addWidget(checkBoxDisableDialogButtonBoxIcons);

        auto checkBoxSuppressHighlightColorFocusedTreeViewItemsText =
            tr("Suppress special highlight color for focused tree view items");
        auto checkBoxSuppressHighlightColorFocusedTreeViewItems =
            new QCheckBox(
                checkBoxSuppressHighlightColorFocusedTreeViewItemsText,
                this->m_widget);
        layout->addWidget(checkBoxSuppressHighlightColorFocusedTreeViewItems);

        layout->addStretch();
        this->m_widget->setLayout(layout);
    }
    return this->m_widget;
}

void OptionsPage::apply() {}

void OptionsPage::finish() {}

} // namespace OneDark::Internal
