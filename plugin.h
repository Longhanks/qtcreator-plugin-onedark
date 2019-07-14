#pragma once

#include "settings.h"

#include <extensionsystem/iplugin.h>

#include <QStringList>

namespace OneDark::Internal {

class OptionsPage;

class OneDarkPlugin : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE
                          "onedark.json")

public:
    OneDarkPlugin();
    bool initialize(const QStringList &arguments,
                    QString *errorString) override;
    void extensionsInitialized() override {}

private:
    OptionsPage *m_optionsPage = nullptr;
    Settings m_settings;

    void settingsChanged(const Settings &settings);
};

} // namespace OneDark::Internal
