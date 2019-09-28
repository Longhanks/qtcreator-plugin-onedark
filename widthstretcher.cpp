#include "widthstretcher.h"

#include <QPropertyAnimation>
#include <QWidget>

namespace OneDark::Internal {

static inline QEasingCurve makeBezierCurve() noexcept {
    auto curve = QEasingCurve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(
        QPointF(0.25, 0.1), QPointF(0.25, 1.0), QPointF(1.0, 1.0));
    return curve;
}

WidthStretcher::WidthStretcher(QWidget *target, QObject *parent) noexcept
    : QObject(parent) {
    this->m_target = target;
}

double WidthStretcher::scaleFactor() const noexcept {
    return this->m_scaleFactor;
}

void WidthStretcher::setScaleFactor(double value) noexcept {
    this->m_scaleFactor = value;
    this->m_target->update();
}

void WidthStretcher::startScale() noexcept {
    this->m_scaleFactor = 0.0;
    auto *animation = new QPropertyAnimation(this, "scaleFactor");
    animation->setEasingCurve(makeBezierCurve());
    animation->setDuration(250);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

} // namespace OneDark::Internal
