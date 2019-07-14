#pragma once

#include "settings.h"

#include <QProxyStyle>

class QStyleOptionTab;

namespace OneDark::Internal {

class OneDarkProxyStyle : public QProxyStyle {
    Q_OBJECT

public:
    OneDarkProxyStyle(QStyle *style = nullptr);
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

    void setSettings(const Settings &settings);

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
