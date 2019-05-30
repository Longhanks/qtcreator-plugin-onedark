#include "settings.h"

#include <QSettings>

namespace OneDark::Internal {

void Settings::save(QSettings *settings) const {
    settings->beginGroup("OneDarkPlugin");
    settings->setValue("enableTabBarTheme", this->enableTabBarTheme);
    settings->setValue("hideMnemonics", this->hideMnemonics);
    settings->setValue("disableDialogButtonBoxIcons",
                       this->disableDialogButtonBoxIcons);
    settings->setValue("suppressHighlightColorFocusedTreeViewItems",
                       this->suppressHighlightColorFocusedTreeViewItems);
    settings->endGroup();
    settings->sync();
}

void Settings::load(QSettings *settings) {
    settings->beginGroup("OneDarkPlugin");
    this->enableTabBarTheme =
        settings->value("enableTabBarTheme", true).toBool();
    this->hideMnemonics = settings->value("hideMnemonics", true).toBool();
    this->disableDialogButtonBoxIcons =
        settings->value("disableDialogButtonBoxIcons", true).toBool();
    this->suppressHighlightColorFocusedTreeViewItems =
        settings->value("suppressHighlightColorFocusedTreeViewItems", true)
            .toBool();
    settings->endGroup();
}

bool Settings::equals(const Settings &other) const {
    return (this->enableTabBarTheme == other.enableTabBarTheme) &&
           (this->hideMnemonics == other.hideMnemonics) &&
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
