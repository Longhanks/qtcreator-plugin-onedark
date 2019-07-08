#pragma once

class QSettings;

namespace OneDark::Internal {

class Settings {

public:
    bool enableTabBarTheme = true;
    bool forceHideMnemonics = true;
    bool disableDialogButtonBoxIcons = true;
    bool suppressHighlightColorFocusedTreeViewItems = true;

    void save(QSettings *settings) const;
    void load(QSettings *settings);
    bool equals(const Settings &other) const;
};

bool operator==(Settings &s1, Settings &s2);
bool operator!=(Settings &s1, Settings &s2);

} // namespace OneDark::Internal
