#include "plugin.h"

#include <QApplication>
#include <QStyleOption>

inline void init_resource() {
    Q_INIT_RESOURCE(onedark);
}

namespace OneDark::Internal {

static void replaceSingleAmpersands(QString &s) {
    int lastIndex = 0;
    while (true) {
        lastIndex = s.indexOf("&", lastIndex);
        if (lastIndex == -1) {
            return;
        }

        if (s[lastIndex + 1] != '&') {
            s.remove(lastIndex, 1);
            lastIndex = 0;
        } else {
            int toSkip = 1;
            while (s[lastIndex + toSkip] == '&') {
                toSkip += 1;
            }
            lastIndex += toSkip;
        }
    }
}

OneDarkPlugin::OneDarkPlugin() {
    init_resource();
}

bool OneDarkPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                               [[maybe_unused]] QString *errorString) {

    QApplication::setStyle(new OneDarkProxyStyle(QApplication::style()));
    QApplication::style()->moveToThread(QApplication::instance()->thread());
    qApp->setStyleSheet(
        "QTabBar::close-button { image: url(:/x.svg); } "
        "QTabBar::close-button:hover { image: url(:/x-hover.svg); } "
        "QTabBar::tab { padding: 0px 20px; margin: 0px; min-width: 128px; "
        "height: 32px; background: #21252B; border-left: 1px solid #181A20; "
        "border-right: 1px solid #21252B; border-top: 2px solid #21252b; "
        "color: #5c6370; } QTabBar::tab:selected { background: #282c34; "
        "color: #abb2bf; border-top: 2px solid #4b7ff0} QTabBar::tab:last { "
        "border-right: 1px solid #181A20; } QTabBar::tab:selected:!last { "
        "border-right: 1px solid #282c34; } QTabBar::tab:selected:only-one, "
        "QTabBar::tab:only-one { border-right: 1px solid #181A20; } QTabBar{ "
        "height:32px; background-color: #21252B; border: 1px solid #181A20; "
        "border-top:none; }");
    return true;
}

OneDarkProxyStyle::OneDarkProxyStyle(QStyle *style) : QProxyStyle(style) {}

void OneDarkProxyStyle::drawControl(ControlElement element,
                                    const QStyleOption *option,
                                    QPainter *painter,
                                    const QWidget *widget) const {
    if (!qApp->queryKeyboardModifiers().testFlag(Qt::AltModifier)) {
        if (element == QStyle::CE_MenuItem ||
            element == QStyle::CE_MenuBarItem) {
            if (const QStyleOptionMenuItem *styleOptionMenuItem =
                    qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
                auto nonConstStyleOptionMenuItem =
                    const_cast<QStyleOptionMenuItem *>(styleOptionMenuItem);
                replaceSingleAmpersands(nonConstStyleOptionMenuItem->text);
                QProxyStyle::drawControl(element, option, painter, widget);
                return;
            }
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

int OneDarkProxyStyle::styleHint(StyleHint hint,
                                 const QStyleOption *opt,
                                 const QWidget *widget,
                                 QStyleHintReturn *returnData) const {
    if (hint == QStyle::SH_UnderlineShortcut) {
        if (qApp->queryKeyboardModifiers().testFlag(Qt::AltModifier)) {
            return QProxyStyle::styleHint(hint, opt, widget, returnData);
        }
        return 0;
    }

    if (hint == QStyle::SH_MenuBar_AltKeyNavigation) {
        if (qApp->queryKeyboardModifiers().testFlag(Qt::AltModifier)) {
            return QProxyStyle::styleHint(hint, opt, widget, returnData);
        }
        return 0;
    }

    if (hint == QStyle::SH_DialogButtonBox_ButtonsHaveIcons) {
        return 0;
    }

    return QProxyStyle::styleHint(hint, opt, widget, returnData);
}

void OneDarkProxyStyle::drawPrimitive(PrimitiveElement element,
                                      const QStyleOption *option,
                                      QPainter *painter,
                                      const QWidget *widget) const {
    if (element == QStyle::PE_FrameTabBarBase) {
        return;
    }
    if (element == QStyle::PE_PanelItemViewRow) {
        if (const QStyleOptionViewItem *styleOptionViewItem =
                qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
            QStyleOptionViewItem *nonConstStyleOptionViewItem =
                const_cast<QStyleOptionViewItem *>(styleOptionViewItem);
            nonConstStyleOptionViewItem->state =
                nonConstStyleOptionViewItem->state & ~QStyle::State_HasFocus;
        }
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

} // namespace OneDark::Internal
