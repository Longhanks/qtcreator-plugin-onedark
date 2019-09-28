#pragma once

#include <QObject>

namespace OneDark::Internal {

class FaderHolder final : public QObject {
    Q_OBJECT
    Q_PROPERTY(double fader READ fader WRITE setFader)

public:
    explicit FaderHolder(QWidget *target, QObject *parent = nullptr) noexcept;
    FaderHolder(const FaderHolder &) = delete;
    FaderHolder &operator=(const FaderHolder &) = delete;
    FaderHolder(FaderHolder &&) = delete;
    FaderHolder &operator=(FaderHolder &&) = delete;
    ~FaderHolder() noexcept override = default;

    double fader() const noexcept;
    void setFader(double value) noexcept;

    void startFadeIn() noexcept;
    void startFadeOut() noexcept;

private:
    double m_fader = 0.0;
    QWidget *m_target = nullptr;
};

} // namespace OneDark::Internal
