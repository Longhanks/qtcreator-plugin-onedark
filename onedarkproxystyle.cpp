#include "onedarkproxystyle.h"

#include <QCheckBox>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

#include <qpa/qplatformtheme.h>

namespace OneDark::Internal {

OneDarkProxyStyle::OneDarkProxyStyle(QStyle *style) : QProxyStyle(style) {
    this->m_iconTabCloseNormal = QIcon(":/icons/tab-close-normal.svg");
    this->m_iconTabCloseHover = QIcon(":/icons/tab-close-hover.svg");
}

void OneDarkProxyStyle::drawPrimitive(PrimitiveElement element,
                                      const QStyleOption *option,
                                      QPainter *painter,
                                      const QWidget *widget) const {
    if (this->m_settings.enableTabBarTheme) {
        if (element == PE_IndicatorTabClose) {
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
        if (element == PE_FrameTabBarBase) {
            if (const QStyleOptionTabBarBase *tbb =
                    qstyleoption_cast<const QStyleOptionTabBarBase *>(
                        option)) {
                const_cast<QStyleOptionTabBarBase *>(tbb)->palette.setColor(
                    QPalette::Light, QColor(33, 37, 43));
            }
        }
    }

    if (this->m_settings.suppressHighlightColorFocusedTreeViewItems) {
        if (element == PE_PanelItemViewRow) {
            if (const QStyleOptionViewItem *styleOptionViewItem =
                    qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
                QStyleOptionViewItem *nonConstStyleOptionViewItem =
                    const_cast<QStyleOptionViewItem *>(styleOptionViewItem);
                nonConstStyleOptionViewItem->state =
                    nonConstStyleOptionViewItem->state & ~State_HasFocus;
            }
        }
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void OneDarkProxyStyle::drawControl(ControlElement element,
                                    const QStyleOption *option,
                                    QPainter *painter,
                                    const QWidget *widget) const {
    if (element == CE_TabBarTab) {
        this->drawControl(CE_TabBarTabShape, option, painter, widget);
        this->drawControl(CE_TabBarTabLabel, option, painter, widget);
        return;
    }

    if (element == CE_MenuBarEmptyArea) {
        return;
    }

    [&element, option, this]() {
        if (element != CE_MenuItem && element != CE_MenuBarItem) {
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

    if (element == CE_TabBarTabShape && this->m_settings.enableTabBarTheme) {
        painter->save();
        if (const QStyleOptionTab *tab =
                qstyleoption_cast<const QStyleOptionTab *>(option)) {
            bool selected = tab->state & State_Selected;
            bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
            bool rtlHorTabs = (tab->direction == Qt::RightToLeft &&
                               (tab->shape == QTabBar::RoundedNorth ||
                                tab->shape == QTabBar::RoundedSouth));
            bool lastTab =
                ((!rtlHorTabs && tab->position == QStyleOptionTab::End) ||
                 (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
            int overlap = this->proxy()->pixelMetric(
                PM_TabBarTabOverlap, option, widget);
            const_cast<QStyleOptionTab *>(tab)->rect.adjust(0, 0, 0, -overlap);

            painter->setPen(Qt::NoPen);
            painter->setRenderHint(QPainter::Antialiasing, false);
            painter->save();
            if (selected) {
                painter->setBrush(QBrush(QColor(40, 44, 52)));
            } else {
                painter->setBrush(QBrush(QColor(33, 37, 43)));
            }

            painter->drawRect(option->rect);
            painter->restore();
            painter->fillRect(option->rect.left(),
                              option->rect.top(),
                              1,
                              option->rect.height(),
                              QColor(24, 26, 32));
            if (lastTab || onlyOne) {
                painter->fillRect(option->rect.right(),
                                  option->rect.top(),
                                  1,
                                  option->rect.height(),
                                  QColor(24, 26, 32));
            }
            if (selected) {
                painter->fillRect(option->rect.left() + 1,
                                  option->rect.top(),
                                  option->rect.width() - 2,
                                  2,
                                  QColor(75, 127, 240));
            } else {
                painter->fillRect(option->rect.left(),
                                  option->rect.top(),
                                  option->rect.width(),
                                  1,
                                  QColor(24, 26, 32));
            }
        }
        painter->restore();
        return;
    }

    if (element == CE_TabBarTabLabel && this->m_settings.enableTabBarTheme) {
        if (const QStyleOptionTab *tab =
                qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect tr = tab->rect;
            bool verticalTabs = tab->shape == QTabBar::RoundedEast ||
                                tab->shape == QTabBar::RoundedWest ||
                                tab->shape == QTabBar::TriangularEast ||
                                tab->shape == QTabBar::TriangularWest;
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!this->proxy()->styleHint(
                    SH_UnderlineShortcut, option, widget)) {
                alignment |= Qt::TextHideMnemonic;
            }
            if (verticalTabs) {
                painter->save();
                int newX, newY, newRot;
                if (tab->shape == QTabBar::RoundedEast ||
                    tab->shape == QTabBar::TriangularEast) {
                    newX = tr.width() + tr.x();
                    newY = tr.y();
                    newRot = 90;
                } else {
                    newX = tr.x();
                    newY = tr.y() + tr.height();
                    newRot = -90;
                }
                QTransform m = QTransform::fromTranslate(newX, newY);
                m.rotate(newRot);
                painter->setTransform(m, true);
            }
            QRect iconRect;
            this->tabLayout(tab, widget, &tr, &iconRect);
            if (!tab->icon.isNull()) {
                auto window = [widget]() {
                    return widget ? widget->window()->windowHandle() : nullptr;
                }();
                QPixmap tabIcon = tab->icon.pixmap(
                    window,
                    tab->iconSize,
                    (tab->state & State_Enabled) ? QIcon::Normal
                                                 : QIcon::Disabled,
                    (tab->state & State_Selected) ? QIcon::On : QIcon::Off);
                painter->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
            }
            QPalette textPalette = tab->palette;
            bool selected = tab->state & State_Selected;
            if (selected) {
                textPalette.setColor(QPalette::WindowText,
                                     QColor(171, 178, 191));
            } else {
                textPalette.setColor(QPalette::WindowText,
                                     QColor(92, 99, 112));
            }
            const_cast<QStyleOptionTab *>(tab)->palette = textPalette;
            this->proxy()->drawItemText(painter,
                                        tr,
                                        alignment,
                                        tab->palette,
                                        tab->state & State_Enabled,
                                        tab->text,
                                        QPalette::WindowText);
            if (verticalTabs) {
                painter->restore();
            }
            return;
        }
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}

QRect OneDarkProxyStyle::subElementRect(QStyle::SubElement element,
                                        const QStyleOption *option,
                                        const QWidget *widget) const {
    if (element == SE_TabBarTabRightButton ||
        element == SE_TabBarTabLeftButton) {
        if (const QStyleOptionTab *tab =
                qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect r;
            bool selected = tab->state & State_Selected;
            int verticalShift =
                this->pixelMetric(PM_TabBarTabShiftVertical, tab, widget);
            int horizontalShift =
                this->pixelMetric(PM_TabBarTabShiftHorizontal, tab, widget);
            int hpadding =
                this->pixelMetric(PM_TabBarTabHSpace, option, widget) / 2;
            hpadding = qMax(hpadding, 4);
            bool verticalTabs = tab->shape == QTabBar::RoundedEast ||
                                tab->shape == QTabBar::RoundedWest ||
                                tab->shape == QTabBar::TriangularEast ||
                                tab->shape == QTabBar::TriangularWest;
            QRect tr = tab->rect;
            if (tab->shape == QTabBar::RoundedSouth ||
                tab->shape == QTabBar::TriangularSouth) {
                verticalShift = -verticalShift;
            }
            if (verticalTabs) {
                qSwap(horizontalShift, verticalShift);
                horizontalShift *= -1;
                verticalShift *= -1;
            }
            if (tab->shape == QTabBar::RoundedWest ||
                tab->shape == QTabBar::TriangularWest) {
                horizontalShift = -horizontalShift;
            }
            tr.adjust(0, 0, horizontalShift, verticalShift);
            if (selected) {
                tr.setBottom(tr.bottom() - verticalShift);
                tr.setRight(tr.right() - horizontalShift);
            }
            QSize size = (element == SE_TabBarTabLeftButton)
                             ? tab->leftButtonSize
                             : tab->rightButtonSize;
            int w = size.width();
            int h = size.height();
            int midHeight =
                static_cast<int>(std::ceil(float(tr.height() - h) / 2));
            int midWidth = ((tr.width() - w) / 2);
            bool atTheTop = true;
            switch (tab->shape) {
            case QTabBar::RoundedWest:
                [[fallthrough]];
            case QTabBar::TriangularWest:
                atTheTop = (element == SE_TabBarTabLeftButton);
                break;
            case QTabBar::RoundedEast:
                [[fallthrough]];
            case QTabBar::TriangularEast:
                atTheTop = (element == SE_TabBarTabRightButton);
                break;
            default:
                if (element == SE_TabBarTabLeftButton) {
                    r = QRect(tab->rect.x() + hpadding, midHeight, w, h);
                } else {
                    r = QRect(
                        tab->rect.right() - w - hpadding, midHeight, w, h);
                }
                r = visualRect(tab->direction, tab->rect, r);
            }
            if (verticalTabs) {
                if (atTheTop) {
                    r = QRect(midWidth,
                              tr.y() + tab->rect.height() - hpadding - h,
                              w,
                              h);
                } else {
                    r = QRect(midWidth, tr.y() + hpadding, w, h);
                }
            }
            return r;
        }
    }
    return QProxyStyle::subElementRect(element, option, widget);
}

int OneDarkProxyStyle::styleHint(StyleHint hint,
                                 const QStyleOption *opt,
                                 const QWidget *widget,
                                 QStyleHintReturn *returnData) const {
    if (this->m_settings.forceHideMnemonics) {
        if (hint == SH_UnderlineShortcut) {
            return 0;
        }
    }

    if (this->m_settings.disableDialogButtonBoxIcons) {
        if (hint == SH_DialogButtonBox_ButtonsHaveIcons) {
            return 0;
        }
    }

    return QProxyStyle::styleHint(hint, opt, widget, returnData);
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
    if (metric == PM_TabBarTabShiftVertical) {
        return 0;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
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

void OneDarkProxyStyle::setSettings(const Settings &settings) {
    this->m_settings = settings;
}

void OneDarkProxyStyle::tabLayout(const QStyleOptionTab *opt,
                                  const QWidget *widget,
                                  QRect *textRect,
                                  QRect *iconRect) const {
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast ||
                        opt->shape == QTabBar::RoundedWest ||
                        opt->shape == QTabBar::TriangularEast ||
                        opt->shape == QTabBar::TriangularWest;
    if (verticalTabs) {
        tr.setRect(0, 0, tr.height(), tr.width());
    }
    int hpadding =
        this->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
    int vpadding =
        this->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    tr.adjust(hpadding, -vpadding, -hpadding, vpadding);
    if (!opt->leftButtonSize.isEmpty()) {
        tr.setLeft(tr.left() + 4 +
                   (verticalTabs ? opt->leftButtonSize.height()
                                 : opt->leftButtonSize.width()));
    }
    if (!opt->rightButtonSize.isEmpty()) {
        tr.setRight(tr.right() - 4 -
                    (verticalTabs ? opt->rightButtonSize.height()
                                  : opt->rightButtonSize.width()));
    }
    if (!opt->icon.isNull()) {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid()) {
            int iconExtent = this->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(
            iconSize,
            (opt->state & QStyle::State_Enabled) ? QIcon::Normal
                                                 : QIcon::Disabled,
            (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
        tabIconSize = QSize(qMin(tabIconSize.width(), iconSize.width()),
                            qMin(tabIconSize.height(), iconSize.height()));
        const int offsetX = (iconSize.width() - tabIconSize.width()) / 2;
        *iconRect = QRect(tr.left() + offsetX,
                          tr.center().y() - tabIconSize.height() / 2,
                          tabIconSize.width(),
                          tabIconSize.height());
        if (!verticalTabs) {
            *iconRect = this->visualRect(opt->direction, opt->rect, *iconRect);
        }
        tr.setLeft(tr.left() + tabIconSize.width() + 4);
    }
    if (!verticalTabs) {
        tr = this->visualRect(opt->direction, opt->rect, tr);
    }
    *textRect = tr;
}

} // namespace OneDark::Internal
