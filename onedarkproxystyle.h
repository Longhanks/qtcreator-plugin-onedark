#pragma once

#include "settings.h"

#include <QProxyStyle>

class QStyleOptionTab;
class QTabBar;

namespace OneDark::Internal {

class FaderHolder;
class WidthStretcher;

class OneDarkProxyStyle : public QProxyStyle {
    Q_OBJECT

public:
    OneDarkProxyStyle(QStyle *style = nullptr);
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

    void setSettings(const Settings &settings);
    static FaderHolder *faderForTabBarIndex(QTabBar *tabBar,
                                            std::size_t index);
    static WidthStretcher *widthStretcherForTabBar(QTabBar *tabBar);

private:
    Settings m_settings;

    QIcon m_iconTabCloseNormal;
    QIcon m_iconTabCloseHover;

    void tabLayout(const QStyleOptionTab *opt,
                   const QWidget *widget,
                   QRect *textRect,
                   QRect *iconRect) const;
};

} // namespace OneDark::Internal
