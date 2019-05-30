#include "plugin.h"

#include "optionspage.h"

#include <coreplugin/icore.h>

#include <QApplication>
#include <QFile>
#include <QStyleOption>
#include <QTimer>

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
    this->m_settings.load(Core::ICore::settings());

    auto oneDarkStyle = new OneDarkProxyStyle(QApplication::style());
    oneDarkStyle->setSettings(this->m_settings);
    QApplication::setStyle(oneDarkStyle);
    QApplication::style()->moveToThread(QApplication::instance()->thread());
    QTimer::singleShot(0, qApp, [this]() {
        this->setOneDarkTabsEnabled(this->m_settings.enableTabBarTheme);
    });

    this->m_optionsPage = new OptionsPage(this->m_settings, this);

    connect(m_optionsPage,
            &OptionsPage::settingsChanged,
            this,
            &OneDarkPlugin::settingsChanged);

    connect(Core::ICore::instance(),
            &Core::ICore::saveSettingsRequested,
            this,
            [this] { this->m_settings.save(Core::ICore::settings()); });

    return true;
}

void OneDarkPlugin::settingsChanged(const Settings &settings) {
    settings.save(Core::ICore::settings());
    this->m_settings = settings;
    this->setOneDarkTabsEnabled(this->m_settings.enableTabBarTheme);
    this->m_optionsPage->setSettings(this->m_settings);
    static_cast<OneDarkProxyStyle *>(QApplication::style())
        ->setSettings(this->m_settings);
}

void OneDarkPlugin::setOneDarkTabsEnabled(bool enabled) {
    QString qss = [enabled]() -> QString {
        if (!enabled) {
            return QString();
        }
        auto qssFile = QFile(":/onedark.qss");
        qssFile.open(QIODevice::ReadOnly | QIODevice::Text);
        auto qssStream = QTextStream(&qssFile);
        return qssStream.readAll();
    }();

    auto tabBars = Core::ICore::mainWindow()->findChildren<QTabBar *>();
    for (QTabBar *t : tabBars) {
        if (t->window() != Core::ICore::mainWindow()) {
            continue;
        }
        t->setStyleSheet(qss);
    }
}

OneDarkProxyStyle::OneDarkProxyStyle(QStyle *style) : QProxyStyle(style) {}

void OneDarkProxyStyle::drawControl(ControlElement element,
                                    const QStyleOption *option,
                                    QPainter *painter,
                                    const QWidget *widget) const {
    if (this->m_settings.hideMnemonics) {
        if (!qApp->queryKeyboardModifiers().testFlag(Qt::AltModifier)) {
            if (element == QStyle::CE_MenuItem ||
                element == QStyle::CE_MenuBarItem) {
                if (const QStyleOptionMenuItem *styleOptionMenuItem =
                        qstyleoption_cast<const QStyleOptionMenuItem *>(
                            option)) {
                    auto nonConstStyleOptionMenuItem =
                        const_cast<QStyleOptionMenuItem *>(
                            styleOptionMenuItem);
                    replaceSingleAmpersands(nonConstStyleOptionMenuItem->text);
                    QProxyStyle::drawControl(element, option, painter, widget);
                    return;
                }
            }
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

int OneDarkProxyStyle::styleHint(StyleHint hint,
                                 const QStyleOption *opt,
                                 const QWidget *widget,
                                 QStyleHintReturn *returnData) const {
    if (this->m_settings.hideMnemonics) {
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
    }

    if (this->m_settings.disableDialogButtonBoxIcons) {
        if (hint == QStyle::SH_DialogButtonBox_ButtonsHaveIcons) {
            return 0;
        }
    }

    return QProxyStyle::styleHint(hint, opt, widget, returnData);
}

void OneDarkProxyStyle::drawPrimitive(PrimitiveElement element,
                                      const QStyleOption *option,
                                      QPainter *painter,
                                      const QWidget *widget) const {
    if (this->m_settings.enableTabBarTheme) {
        if (element == QStyle::PE_FrameTabBarBase) {
            return;
        }
    }

    if (this->m_settings.suppressHighlightColorFocusedTreeViewItems) {
        if (element == QStyle::PE_PanelItemViewRow) {
            if (const QStyleOptionViewItem *styleOptionViewItem =
                    qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
                QStyleOptionViewItem *nonConstStyleOptionViewItem =
                    const_cast<QStyleOptionViewItem *>(styleOptionViewItem);
                nonConstStyleOptionViewItem->state =
                    nonConstStyleOptionViewItem->state &
                    ~QStyle::State_HasFocus;
            }
        }
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void OneDarkProxyStyle::setSettings(const Settings &settings) {
    this->m_settings = settings;
}

} // namespace OneDark::Internal
