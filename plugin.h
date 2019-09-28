#pragma once

#include "settings.h"

#include <extensionsystem/iplugin.h>

#include <QStringList>

namespace OneDark::Internal {

class OptionsPage;

class OneDarkPlugin final : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE
                          "onedark.json")

public:
    explicit OneDarkPlugin() noexcept;
    OneDarkPlugin(const OneDarkPlugin &) = delete;
    OneDarkPlugin &operator=(const OneDarkPlugin &) = delete;
    OneDarkPlugin(OneDarkPlugin &&) = delete;
    OneDarkPlugin &operator=(OneDarkPlugin &&) = delete;
    ~OneDarkPlugin() noexcept override;

    bool initialize(const QStringList &arguments,
                    QString *errorString) override;
    void extensionsInitialized() override {}

private:
    OptionsPage *m_optionsPage = nullptr;
    Settings m_settings;

    void onSettingsChanged(Settings settings) noexcept;
};

} // namespace OneDark::Internal
