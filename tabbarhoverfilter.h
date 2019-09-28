#pragma once

#include <QObject>

namespace OneDark::Internal {

class TabHoverFilter final : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;
    TabHoverFilter(const TabHoverFilter &) = delete;
    TabHoverFilter &operator=(const TabHoverFilter &) = delete;
    TabHoverFilter(TabHoverFilter &&) = delete;
    TabHoverFilter &operator=(TabHoverFilter &&) = delete;
    ~TabHoverFilter() noexcept override = default;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    int m_lastHoveredTabIndex = -1;
    bool m_block = false;
};

} // namespace OneDark::Internal
