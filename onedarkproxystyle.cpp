#include "onedarkproxystyle.h"

#include "faderholder.h"
#include "tabbarhoverfilter.h"
#include "widthstretcher.h"

#include <utils/theme/theme.h>

#include <QCheckBox>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

#include <qpa/qplatformtheme.h>

#include <cmath>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace OneDark::Internal {

static std::unordered_map<const QTabBar *, std::vector<FaderHolder *>> *faders;
static std::unordered_map<const QTabBar *, WidthStretcher *> *widthStretchers;
static std::unordered_map<const QTabBar *, TabHoverFilter *> *filters;

OneDarkProxyStyle::OneDarkProxyStyle(QStyle *style) noexcept
    : QProxyStyle(style) {
    this->m_iconTabCloseNormal = QIcon(":/icons/tab-close-normal.svg");
    this->m_iconTabCloseHover = QIcon(":/icons/tab-close-hover.svg");
    faders =
        new std::unordered_map<const QTabBar *, std::vector<FaderHolder *>>;
    widthStretchers =
        new std::unordered_map<const QTabBar *, WidthStretcher *>;
    filters = new std::unordered_map<const QTabBar *, TabHoverFilter *>;
}

OneDarkProxyStyle::~OneDarkProxyStyle() noexcept {
    delete faders;
    delete widthStretchers;
    delete filters;
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
            if (const auto *styleOptionTabBarBase =
                    qstyleoption_cast<const QStyleOptionTabBarBase *>(
                        option)) {
                const_cast<QStyleOptionTabBarBase *>(styleOptionTabBarBase)
                    ->palette.setColor(QPalette::Light, QColor(33, 37, 43));
            }
        }
    }

    if (this->m_settings.suppressHighlightColorFocusedTreeViewItems) {
        if (element == PE_PanelItemViewRow) {
            if (const auto *styleOptionViewItem =
                    qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
                auto *mutableStyleOptionViewItem =
                    const_cast<QStyleOptionViewItem *>(styleOptionViewItem);
                mutableStyleOptionViewItem->state =
                    mutableStyleOptionViewItem->state & ~State_HasFocus;
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
        if (const auto *mbi =
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
                if (mbi->state & State_Enabled &&
                    !(mbi->state & State_Selected) && !isActive) {
                    const_cast<QStyleOptionMenuItem *>(mbi)->palette.setColor(
                        QPalette::ButtonText, QColor(92, 99, 112));
                }
                if (mbi->state & State_Selected) {
                    const auto hoverColor = Utils::creatorTheme()->color(
                        Utils::Theme::FancyToolButtonHoverColor);
                    painter->fillRect(mbi->rect, QBrush(hoverColor));
                }
                this->proxy()->drawItemText(painter,
                                            mbi->rect,
                                            static_cast<int>(alignment),
                                            mbi->palette,
                                            mbi->state & State_Enabled,
                                            mbi->text,
                                            QPalette::ButtonText);
            }
            return;
        }
    }

    if (element == CE_TabBarTabShape && this->m_settings.enableTabBarTheme) {
        painter->save();
        if (const auto *tab =
                qstyleoption_cast<const QStyleOptionTab *>(option)) {
            const bool selected = tab->state & State_Selected;
            const bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
            const bool rtlHorTabs = (tab->direction == Qt::RightToLeft &&
                                     (tab->shape == QTabBar::RoundedNorth ||
                                      tab->shape == QTabBar::RoundedSouth));
            const bool lastTab =
                ((!rtlHorTabs && tab->position == QStyleOptionTab::End) ||
                 (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
            const int overlap = this->proxy()->pixelMetric(
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

            if (!selected) {
                painter->save();
                const auto *tabbar = static_cast<const QTabBar *>(widget);
                const auto currentIndex =
                    tabbar->tabAt(option->rect.topLeft());
                const auto *fader = OneDarkProxyStyle::faderForTabBarIndex(
                    const_cast<QTabBar *>(tabbar),
                    static_cast<std::size_t>(currentIndex));
                const auto hoverColor = [fader] {
                    auto hoverColor_ = QColor(37, 41, 48);
                    hoverColor_.setAlpha(static_cast<int>(hoverColor_.alpha() *
                                                          fader->fader()));
                    return hoverColor_;
                }();
                painter->setBrush(QBrush(hoverColor));
                const auto width = [option, lastTab, onlyOne] {
                    auto width_ = option->rect.width();
                    if (lastTab || onlyOne) {
                        width_ -= 1;
                    }
                    return width_;
                }();
                painter->drawRect(option->rect.left() + 1,
                                  option->rect.top() + 2,
                                  width - 1,
                                  option->rect.height() - 2);
                painter->restore();

                auto *widthStretcher =
                    OneDarkProxyStyle::widthStretcherForTabBar(
                        const_cast<QTabBar *>(tabbar));
                const auto topBarHoverColor = [fader, &hoverColor] {
                    auto topBarHoverColor_ = Utils::creatorTheme()->color(
                        Utils::Theme::FancyToolButtonHoverColor);
                    topBarHoverColor_.setAlpha(
                        static_cast<int>(hoverColor.alpha() * fader->fader()));
                    return topBarHoverColor_;
                }();

                const auto topBarWidth =
                    static_cast<int>((option->rect.width() - 2) *
                                     widthStretcher->scaleFactor());
                const auto topBarTopLeft = static_cast<int>(
                    option->rect.left() + 1 +
                    ((option->rect.width() - 2) / 2) - (topBarWidth / 2));

                painter->fillRect(topBarTopLeft,
                                  option->rect.top(),
                                  topBarWidth,
                                  2,
                                  topBarHoverColor);
            }
        }
        painter->restore();
        return;
    }

    if (element == CE_TabBarTabLabel && this->m_settings.enableTabBarTheme) {
        if (const auto *tab =
                qstyleoption_cast<const QStyleOptionTab *>(option)) {
            const bool verticalTabs = tab->shape == QTabBar::RoundedEast ||
                                      tab->shape == QTabBar::RoundedWest ||
                                      tab->shape == QTabBar::TriangularEast ||
                                      tab->shape == QTabBar::TriangularWest;
            const int alignment = [this, option, widget] {
                int alignment_ = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!this->proxy()->styleHint(
                        SH_UnderlineShortcut, option, widget)) {
                    alignment_ |= Qt::TextHideMnemonic;
                }
                return alignment_;
            }();
            if (verticalTabs) {
                painter->save();
                const auto [newX, newY, newRot] =
                    [tab]() -> std::tuple<int, int, int> {
                    if (tab->shape == QTabBar::RoundedEast ||
                        tab->shape == QTabBar::TriangularEast) {
                        return {tab->rect.width() + tab->rect.x(),
                                tab->rect.y(),
                                90};
                    }
                    return {tab->rect.x(),
                            tab->rect.y() + tab->rect.height(),
                            -90};
                }();
                const auto transform = [](int x, int y, int rot) {
                    auto transform_ = QTransform::fromTranslate(x, y);
                    transform_.rotate(rot);
                    return transform_;
                }(newX, newY, newRot);
                painter->setTransform(transform, true);
            }
            const auto [textRect, iconRect] = this->tabLayout(tab, widget);
            if (!tab->icon.isNull()) {
                auto *window = [widget]() {
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
            const bool selected = tab->state & State_Selected;
            if (selected) {
                textPalette.setColor(QPalette::WindowText,
                                     QColor(171, 178, 191));
            } else {
                textPalette.setColor(QPalette::WindowText,
                                     QColor(92, 99, 112));
            }
            const_cast<QStyleOptionTab *>(tab)->palette = textPalette;
            this->proxy()->drawItemText(painter,
                                        textRect,
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
        if (const auto *tab =
                qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect r;
            const bool selected = tab->state & State_Selected;
            int verticalShift =
                this->pixelMetric(PM_TabBarTabShiftVertical, tab, widget);
            int horizontalShift =
                this->pixelMetric(PM_TabBarTabShiftHorizontal, tab, widget);
            const int hpadding = [this, option, widget] {
                int hpadding_ =
                    this->pixelMetric(PM_TabBarTabHSpace, option, widget) / 2;
                return (hpadding_ < 4) ? 4 : hpadding_;
            }();
            const bool verticalTabs = tab->shape == QTabBar::RoundedEast ||
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
            const QSize size = (element == SE_TabBarTabLeftButton)
                                   ? tab->leftButtonSize
                                   : tab->rightButtonSize;
            const int w = size.width();
            const int h = size.height();
            const int midHeight = static_cast<int>(
                std::ceil(static_cast<float>(tr.height() - h) / 2));
            const int midWidth = ((tr.width() - w) / 2);
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
            case QTabBar::RoundedNorth:
                [[fallthrough]];
            case QTabBar::RoundedSouth:
                [[fallthrough]];
            case QTabBar::TriangularNorth:
                [[fallthrough]];
            case QTabBar::TriangularSouth:
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
    auto *btn = qobject_cast<QPushButton *>(widget);
    if (btn != nullptr) {
        const QString text = QPlatformTheme::removeMnemonics(btn->text());
        btn->setText(text);
    }
    auto *checkbox = qobject_cast<QCheckBox *>(widget);
    if (checkbox != nullptr) {
        const QString text = QPlatformTheme::removeMnemonics(checkbox->text());
        checkbox->setText(text);
    }
    auto *tabBar = qobject_cast<QTabBar *>(widget);
    if (tabBar != nullptr) {
        tabBar->setAttribute(Qt::WidgetAttribute::WA_Hover);
        (*faders)[tabBar] = {};
        (*widthStretchers)[tabBar] = new WidthStretcher(tabBar, this);
        (*filters)[tabBar] = new TabHoverFilter(this);
        tabBar->installEventFilter((*filters)[tabBar]);
    }
    QProxyStyle::polish(widget);
}

void OneDarkProxyStyle::unpolish(QWidget *widget) {
    auto *tabBar = qobject_cast<QTabBar *>(widget);
    if (tabBar != nullptr) {
        TabHoverFilter *filter = (*filters)[tabBar];
        tabBar->removeEventFilter(filter);
        delete filter;
        filters->erase(tabBar);
        WidthStretcher *widthStretcher = (*widthStretchers)[tabBar];
        delete widthStretcher;
        widthStretchers->erase(tabBar);
        std::for_each(std::begin((*faders)[tabBar]),
                      std::end((*faders)[tabBar]),
                      std::default_delete<FaderHolder>());
        faders->erase(tabBar);
        tabBar->setAttribute(Qt::WidgetAttribute::WA_Hover, false);
    }
    QProxyStyle::unpolish(widget);
}

void OneDarkProxyStyle::setSettings(Settings settings) noexcept {
    this->m_settings = std::move(settings);
}

FaderHolder *
OneDarkProxyStyle::faderForTabBarIndex(QTabBar *tabBar,
                                       std::size_t index) noexcept {
    if ((*faders)[tabBar].size() < index + 1) {
        (*faders)[tabBar].resize(index + 1);
    }
    auto *holder = (*faders)[tabBar][index];
    if (holder == nullptr) {
        holder = new FaderHolder(tabBar);
        (*faders)[tabBar][index] = holder;
    }
    return holder;
}

WidthStretcher *
OneDarkProxyStyle::widthStretcherForTabBar(QTabBar *tabBar) noexcept {
    return (*widthStretchers)[tabBar];
}

std::pair<QRect, QRect>
OneDarkProxyStyle::tabLayout(const QStyleOptionTab *opt,
                             const QWidget *widget) const noexcept {
    QRect textRect = opt->rect;
    QRect iconRect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast ||
                        opt->shape == QTabBar::RoundedWest ||
                        opt->shape == QTabBar::TriangularEast ||
                        opt->shape == QTabBar::TriangularWest;
    if (verticalTabs) {
        textRect.setRect(0, 0, textRect.height(), textRect.width());
    }
    const int hpadding =
        this->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
    const int vpadding =
        this->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    textRect.adjust(hpadding, -vpadding, -hpadding, vpadding);
    if (!opt->leftButtonSize.isEmpty()) {
        textRect.setLeft(textRect.left() + 4 +
                         (verticalTabs ? opt->leftButtonSize.height()
                                       : opt->leftButtonSize.width()));
    }
    if (!opt->rightButtonSize.isEmpty()) {
        textRect.setRight(textRect.right() - 4 -
                          (verticalTabs ? opt->rightButtonSize.height()
                                        : opt->rightButtonSize.width()));
    }
    if (!opt->icon.isNull()) {
        const QSize iconSize = [opt, this] {
            QSize iconSize_ = opt->iconSize;
            if (!iconSize_.isValid()) {
                int iconExtent = this->pixelMetric(QStyle::PM_SmallIconSize);
                iconSize_ = QSize(iconExtent, iconExtent);
            }
            return iconSize_;
        }();

        const QSize tabIconSize = [&iconSize, opt] {
            QSize tabIconSize_ = opt->icon.actualSize(
                iconSize,
                (opt->state & QStyle::State_Enabled) ? QIcon::Normal
                                                     : QIcon::Disabled,
                (opt->state & QStyle::State_Selected) ? QIcon::On
                                                      : QIcon::Off);
            tabIconSize_ =
                QSize(qMin(tabIconSize_.width(), iconSize.width()),
                      qMin(tabIconSize_.height(), iconSize.height()));
            return tabIconSize_;
        }();
        const int offsetX = (iconSize.width() - tabIconSize.width()) / 2;
        iconRect = QRect(textRect.left() + offsetX,
                         textRect.center().y() - tabIconSize.height() / 2,
                         tabIconSize.width(),
                         tabIconSize.height());
        if (!verticalTabs) {
            iconRect = this->visualRect(opt->direction, opt->rect, iconRect);
        }
        textRect.setLeft(textRect.left() + tabIconSize.width() + 4);
    }
    if (!verticalTabs) {
        textRect = this->visualRect(opt->direction, opt->rect, textRect);
    }

    return {std::move(textRect), std::move(iconRect)};
}

} // namespace OneDark::Internal
