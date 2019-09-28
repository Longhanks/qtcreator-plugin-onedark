#pragma once

#include <QWidget>

class QCheckBox;

namespace OneDark::Internal {

struct Settings;

class OptionsDialog final : public QWidget {
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr) noexcept;
    OptionsDialog(const OptionsDialog &) = delete;
    OptionsDialog &operator=(const OptionsDialog &) = delete;
    OptionsDialog(OptionsDialog &&) = delete;
    OptionsDialog &operator=(OptionsDialog &&) = delete;
    ~OptionsDialog() noexcept override = default;

    void setSettings(const Settings &settings) noexcept;
    Settings settings() const noexcept;

private:
    QCheckBox *checkBoxEnableTabBarTheme = nullptr;
    QCheckBox *checkBoxForceHideMnemonics = nullptr;
    QCheckBox *checkBoxDisableDialogButtonBoxIcons = nullptr;
    QCheckBox *checkBoxSuppressHighlightColorFocusedTreeViewItems = nullptr;
};

} // namespace OneDark::Internal
