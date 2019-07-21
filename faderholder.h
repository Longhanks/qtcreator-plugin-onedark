#pragma once

#include <QObject>

namespace OneDark::Internal {

class FaderHolder : public QObject {
    Q_OBJECT
    Q_PROPERTY(double fader READ fader WRITE setFader)

public:
    FaderHolder(QWidget *target, QObject *parent = nullptr);

    double fader() const;
    void setFader(double value);

    void startFadeIn();
    void startFadeOut();

private:
    double m_fader = 0.0;
    QWidget *m_target;
};

} // namespace OneDark::Internal
