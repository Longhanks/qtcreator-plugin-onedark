#include "plugin.h"

#include "onedarkproxystyle.h"
#include "optionspage.h"

#include <coreplugin/icore.h>

#include <QApplication>

static void init_resource() {
    // Note: Q_INIT_RESOURCE must be called form the global namespace
    Q_INIT_RESOURCE(onedark);
}

static void cleanup_resource() {
    // Note: Q_CLEANUP_RESOURCE must be called form the global namespace
    Q_CLEANUP_RESOURCE(onedark);
}

namespace OneDark::Internal {

OneDarkPlugin::OneDarkPlugin() noexcept {
    init_resource();
}

OneDarkPlugin::~OneDarkPlugin() noexcept {
    delete this->m_optionsPage;
    this->m_optionsPage = nullptr;
    cleanup_resource();
}

bool OneDarkPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                               [[maybe_unused]] QString *errorString) {
    this->m_settings.load(*Core::ICore::settings());

    auto *oneDarkStyle = new OneDarkProxyStyle(QApplication::style());
    oneDarkStyle->setSettings(this->m_settings);
    QApplication::setStyle(oneDarkStyle);
    QApplication::style()->moveToThread(QApplication::instance()->thread());

    this->m_optionsPage = new OptionsPage(this->m_settings, this);

    QObject::connect(m_optionsPage,
                     &OptionsPage::settingsChanged,
                     this,
                     &OneDarkPlugin::onSettingsChanged);

    QObject::connect(
        Core::ICore::instance(),
        &Core::ICore::saveSettingsRequested,
        this,
        [this] { this->m_settings.save(*Core::ICore::settings()); });

    return true;
}

void OneDarkPlugin::onSettingsChanged(Settings settings) noexcept {
    settings.save(*Core::ICore::settings());
    this->m_settings = std::move(settings);
    auto *style = qobject_cast<OneDarkProxyStyle *>(QApplication::style());
    if (style != nullptr) {
        style->setSettings(this->m_settings);
    }
}

} // namespace OneDark::Internal
