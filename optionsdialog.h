#pragma once

#include <QWidget>

class QCheckBox;

namespace OneDark::Internal {

class Settings;

class OptionsDialog : public QWidget {
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    ~OptionsDialog() override = default;

    void setSettings(const Settings &settings);
    Settings settings();

private:
    QCheckBox *checkBoxEnableTabBarTheme = nullptr;
    QCheckBox *checkBoxHideMnemonics = nullptr;
    QCheckBox *checkBoxDisableDialogButtonBoxIcons = nullptr;
    QCheckBox *checkBoxSuppressHighlightColorFocusedTreeViewItems = nullptr;
};

} // namespace OneDark::Internal
