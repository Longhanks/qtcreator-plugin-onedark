#pragma once

#include <QWidget>

class QCheckBox;

namespace OneDark::Internal {

class Settings;

class OptionsDialog final : public QWidget {
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr) noexcept;

    void setSettings(const Settings &settings) noexcept;
    Settings settings() const noexcept;

private:
    QCheckBox *checkBoxEnableTabBarTheme = nullptr;
    QCheckBox *checkBoxForceHideMnemonics = nullptr;
    QCheckBox *checkBoxDisableDialogButtonBoxIcons = nullptr;
    QCheckBox *checkBoxSuppressHighlightColorFocusedTreeViewItems = nullptr;
};

} // namespace OneDark::Internal
