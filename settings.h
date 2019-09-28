#pragma once

class QSettings;

namespace OneDark::Internal {

struct Settings final {
    bool enableTabBarTheme = true;
    bool forceHideMnemonics = true;
    bool disableDialogButtonBoxIcons = true;
    bool suppressHighlightColorFocusedTreeViewItems = true;

    void save(QSettings &settings) const noexcept;
    void load(QSettings &settings) noexcept;
    bool equals(const Settings &other) const noexcept;
};

bool operator==(Settings &s1, Settings &s2);
bool operator!=(Settings &s1, Settings &s2);

} // namespace OneDark::Internal
