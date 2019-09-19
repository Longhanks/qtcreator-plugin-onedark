#include "optionspage.h"

#include "optionsdialog.h"

namespace OneDark::Internal {

OptionsPage::OptionsPage(const Settings &settings, QObject *parent) noexcept
    : IOptionsPage(parent, true) {
    this->setSettings(settings);
    this->setId("OneDarkSettings");
    this->setDisplayName("General");
    this->setCategory("OneDark");
    this->setDisplayCategory("OneDark");
    auto iconMask = QStringLiteral(":/icons/atom.png");
    auto iconColor = Utils::Theme::PanelTextColorDark;
    auto iconStyle = Utils::Icon::Tint;
    this->setCategoryIcon(Utils::Icon({{iconMask, iconColor}}, iconStyle));
}

void OptionsPage::setSettings(Settings settings) noexcept {
    this->m_settings = std::move(settings);
}

QWidget *OptionsPage::widget() {
    if (!this->m_widget) {
        this->m_widget = new OptionsDialog;
        this->m_widget->setSettings(this->m_settings);
    }
    return this->m_widget;
}

void OptionsPage::apply() {
    Settings newSettings = this->m_widget->settings();

    if (newSettings != this->m_settings) {
        this->m_settings = std::move(newSettings);
        emit settingsChanged(this->m_settings);
    }
}

void OptionsPage::finish() {
    delete this->m_widget;
}

} // namespace OneDark::Internal
