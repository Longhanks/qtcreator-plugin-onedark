#pragma once

#include "settings.h"

#include <extensionsystem/iplugin.h>

#include <QProxyStyle>
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
    void setOneDarkTabsEnabled(bool enabled);
};

class OneDarkProxyStyle : public QProxyStyle {
    Q_OBJECT

public:
    OneDarkProxyStyle(QStyle *style = nullptr);
    void drawControl(ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget = nullptr) const override;
    int styleHint(StyleHint hint,
                  const QStyleOption *opt = nullptr,
                  const QWidget *widget = nullptr,
                  QStyleHintReturn *returnData = nullptr) const override;
    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget = nullptr) const override;
    int pixelMetric(PixelMetric metric,
                    const QStyleOption *option = nullptr,
                    const QWidget *widget = nullptr) const override;

    void setSettings(const Settings &settings);

private:
    Settings m_settings;
};

} // namespace OneDark::Internal
