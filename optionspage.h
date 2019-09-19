#pragma once

#include "settings.h"

#include <coreplugin/dialogs/ioptionspage.h>

#include <QPointer>

namespace OneDark::Internal {

class OptionsDialog;

class OptionsPage final : public Core::IOptionsPage {
    Q_OBJECT

public:
    explicit OptionsPage(const Settings &settings, QObject *parent) noexcept;

    void setSettings(Settings settings) noexcept;

    QWidget *widget() override;
    void apply() override;
    void finish() override;

signals:
    void settingsChanged(Settings settings);

private:
    QPointer<OptionsDialog> m_widget;
    Settings m_settings;
};

} // namespace OneDark::Internal
