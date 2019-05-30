#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

#include <QPointer>
#include <QWidget>

namespace OneDark::Internal {

class OptionsPage : public Core::IOptionsPage {
    Q_OBJECT

public:
    OptionsPage(QObject *parent);

    QWidget *widget() override;
    void apply() override;
    void finish() override;

private:
    QPointer<QWidget> m_widget;
};

} // namespace OneDark::Internal
