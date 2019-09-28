#include "tabbarhoverfilter.h"

#include "faderholder.h"
#include "onedarkproxystyle.h"
#include "widthstretcher.h"

#include <QHoverEvent>
#include <QPropertyAnimation>
#include <QTabBar>

namespace OneDark::Internal {

bool TabHoverFilter::eventFilter(QObject *watched, QEvent *event) {
    auto *tabBar = qobject_cast<QTabBar *>(watched);
    if (tabBar == nullptr) {
        return false;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        this->m_block = true;
        if (this->m_lastHoveredTabIndex >= 0) {
            auto *holder = OneDarkProxyStyle::faderForTabBarIndex(
                tabBar, static_cast<std::size_t>(this->m_lastHoveredTabIndex));
            holder->setFader(0.0);
        }
        return false;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        this->m_block = false;
        return false;
    }

    if (this->m_block) {
        return false;
    }

    if (event->type() == QEvent::HoverEnter) {
        const auto hoverEvent = static_cast<QHoverEvent *>(event);
        this->m_lastHoveredTabIndex = tabBar->tabAt(hoverEvent->pos());
        if (this->m_lastHoveredTabIndex >= 0) {
            auto *holder = OneDarkProxyStyle::faderForTabBarIndex(
                tabBar, static_cast<std::size_t>(this->m_lastHoveredTabIndex));
            holder->startFadeIn();
            auto *widthStretcher =
                OneDarkProxyStyle::widthStretcherForTabBar(tabBar);
            widthStretcher->startScale();
        }
    } else if (event->type() == QEvent::HoverMove) {
        const auto hoverEvent = static_cast<QHoverEvent *>(event);
        const int newIndex = tabBar->tabAt(hoverEvent->pos());
        if (newIndex != this->m_lastHoveredTabIndex) {
            if (this->m_lastHoveredTabIndex >= 0) {
                auto *holder = OneDarkProxyStyle::faderForTabBarIndex(
                    tabBar,
                    static_cast<std::size_t>(this->m_lastHoveredTabIndex));
                holder->startFadeOut();
            }
            this->m_lastHoveredTabIndex = newIndex;
            if (this->m_lastHoveredTabIndex >= 0) {
                auto *holder = OneDarkProxyStyle::faderForTabBarIndex(
                    tabBar,
                    static_cast<std::size_t>(this->m_lastHoveredTabIndex));
                holder->startFadeIn();
                auto *widthStretcher =
                    OneDarkProxyStyle::widthStretcherForTabBar(tabBar);
                widthStretcher->startScale();
            }
        }
    } else if (event->type() == QEvent::HoverLeave) {
        if (this->m_lastHoveredTabIndex >= 0) {
            auto *holder = OneDarkProxyStyle::faderForTabBarIndex(
                tabBar, static_cast<std::size_t>(this->m_lastHoveredTabIndex));
            holder->startFadeOut();
        }
        this->m_lastHoveredTabIndex = -1;
    }
    return false;
}

} // namespace OneDark::Internal
