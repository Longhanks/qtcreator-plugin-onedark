#include "plugin.h"

#include "onedarkproxystyle.h"
#include "optionspage.h"

#include <coreplugin/icore.h>

#include <QApplication>

inline void init_resource() {
    Q_INIT_RESOURCE(onedark);
}

namespace OneDark::Internal {

OneDarkPlugin::OneDarkPlugin() {
    init_resource();
}

bool OneDarkPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                               [[maybe_unused]] QString *errorString) {
    this->m_settings.load(Core::ICore::settings());

    auto oneDarkStyle = new OneDarkProxyStyle(QApplication::style());
    oneDarkStyle->setSettings(this->m_settings);
    QApplication::setStyle(oneDarkStyle);
    QApplication::style()->moveToThread(QApplication::instance()->thread());

    this->m_optionsPage = new OptionsPage(this->m_settings, this);

    connect(m_optionsPage,
            &OptionsPage::settingsChanged,
            this,
            &OneDarkPlugin::settingsChanged);

    connect(Core::ICore::instance(),
            &Core::ICore::saveSettingsRequested,
            this,
            [this] { this->m_settings.save(Core::ICore::settings()); });

    return true;
}

void OneDarkPlugin::settingsChanged(const Settings &settings) {
    settings.save(Core::ICore::settings());
    this->m_settings = settings;
    this->m_optionsPage->setSettings(this->m_settings);
    static_cast<OneDarkProxyStyle *>(QApplication::style())
        ->setSettings(this->m_settings);
}

} // namespace OneDark::Internal
