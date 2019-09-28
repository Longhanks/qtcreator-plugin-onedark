#pragma once

#include "settings.h"

#include <QProxyStyle>

#include <utility>

class QStyleOptionTab;
class QTabBar;

namespace OneDark::Internal {

class FaderHolder;
class WidthStretcher;

class OneDarkProxyStyle final : public QProxyStyle {
    Q_OBJECT

public:
    explicit OneDarkProxyStyle(QStyle *style = nullptr) noexcept;
    OneDarkProxyStyle(const OneDarkProxyStyle &) = delete;
    OneDarkProxyStyle &operator=(const OneDarkProxyStyle &) = delete;
    OneDarkProxyStyle(OneDarkProxyStyle &&) = delete;
    OneDarkProxyStyle &operator=(OneDarkProxyStyle &&) = delete;
    ~OneDarkProxyStyle() noexcept override;

    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget = nullptr) const override;
    void drawControl(ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget = nullptr) const override;
    QRect subElementRect(SubElement element,
                         const QStyleOption *option,
                         const QWidget *widget) const override;
    int styleHint(StyleHint hint,
                  const QStyleOption *opt = nullptr,
                  const QWidget *widget = nullptr,
                  QStyleHintReturn *returnData = nullptr) const override;
    int pixelMetric(PixelMetric metric,
                    const QStyleOption *option = nullptr,
                    const QWidget *widget = nullptr) const override;
    void polish(QWidget *widget) override;
    using QProxyStyle::polish;
    void unpolish(QWidget *widget) override;
    using QProxyStyle::unpolish;

    void setSettings(Settings settings) noexcept;
    static FaderHolder *faderForTabBarIndex(QTabBar *tabBar,
                                            std::size_t index) noexcept;
    static WidthStretcher *widthStretcherForTabBar(QTabBar *tabBar) noexcept;

private:
    Settings m_settings;

    QIcon m_iconTabCloseNormal;
    QIcon m_iconTabCloseHover;

    std::pair<QRect, QRect> tabLayout(const QStyleOptionTab *opt,
                                      const QWidget *widget) const noexcept;
};

} // namespace OneDark::Internal
