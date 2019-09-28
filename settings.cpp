#include "settings.h"

#include <QSettings>

namespace OneDark::Internal {

void Settings::save(QSettings &settings) const noexcept {
    settings.beginGroup("OneDarkPlugin");
    settings.setValue("enableTabBarTheme", this->enableTabBarTheme);
    settings.setValue("forceHideMnemonics", this->forceHideMnemonics);
    settings.setValue("disableDialogButtonBoxIcons",
                      this->disableDialogButtonBoxIcons);
    settings.setValue("suppressHighlightColorFocusedTreeViewItems",
                      this->suppressHighlightColorFocusedTreeViewItems);
    settings.endGroup();
    settings.sync();
}

void Settings::load(QSettings &settings) noexcept {
    settings.beginGroup("OneDarkPlugin");
    this->enableTabBarTheme =
        settings.value("enableTabBarTheme", true).toBool();
    this->forceHideMnemonics =
        settings.value("forceHideMnemonics", true).toBool();
    this->disableDialogButtonBoxIcons =
        settings.value("disableDialogButtonBoxIcons", true).toBool();
    this->suppressHighlightColorFocusedTreeViewItems =
        settings.value("suppressHighlightColorFocusedTreeViewItems", true)
            .toBool();
    settings.endGroup();
}

bool Settings::equals(const Settings &other) const noexcept {
    return (this->enableTabBarTheme == other.enableTabBarTheme) &&
           (this->forceHideMnemonics == other.forceHideMnemonics) &&
           (this->disableDialogButtonBoxIcons ==
            other.disableDialogButtonBoxIcons) &&
           (this->suppressHighlightColorFocusedTreeViewItems ==
            other.suppressHighlightColorFocusedTreeViewItems);
}

bool operator==(Settings &s1, Settings &s2) {
    return s1.equals(s2);
}

bool operator!=(Settings &s1, Settings &s2) {
    return !s1.equals(s2);
}

} // namespace OneDark::Internal
