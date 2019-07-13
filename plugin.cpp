#include "plugin.h"

#include "optionspage.h"

#include <coreplugin/icore.h>

#include <QApplication>
#include <QCheckBox>
#include <QFile>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QTimer>

#include <qpa/qplatformtheme.h>

inline void init_resource() {
    Q_INIT_RESOURCE(onedark);
}

namespace OneDark::Internal {

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

void OneDarkProxyStyle::polish(QWidget *widget) {
    if (!this->m_settings.forceHideMnemonics) {
        QProxyStyle::polish(widget);
        return;
    }
    auto btn = qobject_cast<QPushButton *>(widget);
    if (btn) {
        auto text = QPlatformTheme::removeMnemonics(btn->text());
        btn->setText(text);
    }
    auto checkbox = qobject_cast<QCheckBox *>(widget);
    if (checkbox) {
        auto text = QPlatformTheme::removeMnemonics(checkbox->text());
        checkbox->setText(text);
    }
    QProxyStyle::polish(widget);
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

OneDarkProxyStyle::OneDarkProxyStyle(QStyle *style) : QProxyStyle(style) {
    this->m_iconTabCloseNormal = QIcon(":/icons/tab-close-normal.svg");
    this->m_iconTabCloseHover = QIcon(":/icons/tab-close-hover.svg");
}

void OneDarkProxyStyle::drawControl(ControlElement element,
                                    const QStyleOption *option,
                                    QPainter *painter,
                                    const QWidget *widget) const {
    if (element == CE_MenuBarEmptyArea) {
        return;
    }

    [&element, option, this]() {
        if (element != QStyle::CE_MenuItem &&
            element != QStyle::CE_MenuBarItem) {
            return;
        }
        if (!this->m_settings.forceHideMnemonics) {
            return;
        }
        auto styleOptionMenuItem =
            qstyleoption_cast<const QStyleOptionMenuItem *>(option);
        if (!styleOptionMenuItem) {
            return;
        }
        QString text =
            QPlatformTheme::removeMnemonics(styleOptionMenuItem->text);
        const_cast<QStyleOptionMenuItem *>(styleOptionMenuItem)->text = text;
    }();

    if (element == CE_MenuBarItem) {
        if (const QStyleOptionMenuItem *mbi =
                qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic |
                             Qt::TextDontClip | Qt::TextSingleLine;
            if (!this->proxy()->styleHint(SH_UnderlineShortcut, mbi, widget)) {
                alignment |= Qt::TextHideMnemonic;
            }
            int iconExtent = this->proxy()->pixelMetric(PM_SmallIconSize);
            QPixmap pix = mbi->icon.pixmap(widget->window()->windowHandle(),
                                           QSize(iconExtent, iconExtent),
                                           (mbi->state & State_Enabled)
                                               ? QIcon::Normal
                                               : QIcon::Disabled);
            if (!pix.isNull()) {
                this->proxy()->drawItemPixmap(
                    painter, mbi->rect, static_cast<int>(alignment), pix);
            } else {
                const bool isActive =
                    widget->parentWidget()->property("active").toBool();
                if (mbi->state & State_Enabled && !isActive) {
                    const_cast<QStyleOptionMenuItem *>(mbi)->palette.setColor(
                        QPalette::ButtonText, Qt::darkGray);
                }
                this->proxy()->drawItemText(painter,
                                            mbi->rect,
                                            static_cast<int>(alignment),
                                            mbi->palette,
                                            mbi->state & State_Enabled,
                                            mbi->text,
                                            QPalette::ButtonText);
                if (mbi->state & State_Selected) {
                    const auto hoverColor = QColor(171, 178, 191, 75);
                    painter->fillRect(mbi->rect, QBrush(hoverColor));
                }
            }
            return;
        }
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}

int OneDarkProxyStyle::styleHint(StyleHint hint,
                                 const QStyleOption *opt,
                                 const QWidget *widget,
                                 QStyleHintReturn *returnData) const {
    if (this->m_settings.forceHideMnemonics) {
        if (hint == QStyle::SH_UnderlineShortcut) {
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
        if (element == QStyle::PE_IndicatorTabClose) {
            auto iconPixmap = [option, this]() -> QPixmap {
                if ((option->state & State_Enabled) &&
                    (option->state & State_MouseOver)) {
                    return this->m_iconTabCloseHover.pixmap(QSize(16, 16));
                }
                return this->m_iconTabCloseNormal.pixmap(QSize(16, 16));
            }();
            this->proxy()->drawItemPixmap(
                painter, option->rect, Qt::AlignCenter, iconPixmap);
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

int OneDarkProxyStyle::pixelMetric(PixelMetric metric,
                                   const QStyleOption *option,
                                   const QWidget *widget) const {
    if (metric == PM_MenuBarPanelWidth) {
        return 0;
    }
    if (metric == PM_MenuBarVMargin) {
        return 5;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

void OneDarkProxyStyle::setSettings(const Settings &settings) {
    this->m_settings = settings;
}

} // namespace OneDark::Internal
