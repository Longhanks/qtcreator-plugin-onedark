#pragma once

#include <QObject>

namespace OneDark::Internal {

class TabHoverFilter : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    int m_lastHoveredTabIndex = -1;
};

} // namespace OneDark::Internal
