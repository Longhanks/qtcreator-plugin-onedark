#include "tabbarhoverfilter.h"

#include "faderholder.h"
#include "onedarkproxystyle.h"

#include <QHoverEvent>
#include <QPropertyAnimation>
#include <QTabBar>

namespace OneDark::Internal {

bool TabHoverFilter::eventFilter(QObject *watched, QEvent *event) {
    auto *tabBar = qobject_cast<QTabBar *>(watched);
    if (tabBar == nullptr) {
        return false;
    }
    if (event->type() == QEvent::HoverEnter) {
        auto hoverEvent = static_cast<QHoverEvent *>(event);
        this->m_lastHoveredTabIndex = tabBar->tabAt(hoverEvent->pos());
        if (this->m_lastHoveredTabIndex >= 0) {
            auto *holder = OneDarkProxyStyle::faderForTabBarIndex(
                tabBar, static_cast<std::size_t>(this->m_lastHoveredTabIndex));
            holder->startFadeIn();
        }
    } else if (event->type() == QEvent::HoverMove) {
        auto hoverEvent = static_cast<QHoverEvent *>(event);
        int newIndex = tabBar->tabAt(hoverEvent->pos());
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
