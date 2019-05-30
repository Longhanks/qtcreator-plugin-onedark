#pragma once

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
};

} // namespace OneDark::Internal
