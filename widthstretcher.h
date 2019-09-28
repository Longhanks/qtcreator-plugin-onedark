#pragma once

#include <QObject>

namespace OneDark::Internal {

class WidthStretcher final : public QObject {
    Q_OBJECT
    Q_PROPERTY(double scaleFactor READ scaleFactor WRITE setScaleFactor)

public:
    WidthStretcher(QWidget *target, QObject *parent = nullptr) noexcept;
    WidthStretcher(const WidthStretcher &) = delete;
    WidthStretcher &operator=(const WidthStretcher &) = delete;
    WidthStretcher(WidthStretcher &&) = delete;
    WidthStretcher &operator=(WidthStretcher &&) = delete;
    ~WidthStretcher() noexcept override = default;

    double scaleFactor() const noexcept;
    void setScaleFactor(double value) noexcept;

    void startScale() noexcept;

private:
    double m_scaleFactor = 100.0;
    QWidget *m_target = nullptr;
};

} // namespace OneDark::Internal
