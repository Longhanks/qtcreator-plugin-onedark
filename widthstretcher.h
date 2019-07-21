#pragma once

#include <QObject>

namespace OneDark::Internal {

class WidthStretcher : public QObject {
    Q_OBJECT
    Q_PROPERTY(double scaleFactor READ scaleFactor WRITE setScaleFactor)

public:
    WidthStretcher(QWidget *target, QObject *parent = nullptr);

    double scaleFactor() const;
    void setScaleFactor(double value);

    void startScale();

private:
    double m_scaleFactor = 100.0;
    QWidget *m_target;
};

} // namespace OneDark::Internal
