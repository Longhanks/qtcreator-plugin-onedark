#include "faderholder.h"

#include <QPropertyAnimation>
#include <QWidget>

namespace OneDark::Internal {

FaderHolder::FaderHolder(QWidget *target, QObject *parent) : QObject(parent) {
    this->m_target = target;
}

double FaderHolder::fader() const {
    return this->m_fader;
}

void FaderHolder::setFader(double value) {
    this->m_fader = value;
    this->m_target->update();
}

void FaderHolder::startFadeIn() {
    auto animation = new QPropertyAnimation(this, "fader");
    animation->setDuration(125);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void FaderHolder::startFadeOut() {
    auto animation = new QPropertyAnimation(this, "fader");
    animation->setDuration(125);
    animation->setEndValue(0.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

} // namespace OneDark::Internal
