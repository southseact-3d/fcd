#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>


#include "FirstStartWidget.h"
#include "ThemeSelectorWidget.h"
#include "GeneralSettingsWidget.h"

#include <App/Application.h>
#include <gsl/pointers>

using namespace StartGui;

FirstStartWidget::FirstStartWidget(QWidget* parent)
    : QGroupBox(parent)
    , _themeSelectorWidget {nullptr}
    , _generalSettingsWidget {nullptr}
    , _welcomeLabel {nullptr}
    , _descriptionLabel {nullptr}
    , _doneButton {nullptr}
{
    setObjectName(QLatin1String("FirstStartWidget"));
    setupUi();
    qApp->installEventFilter(this);
}

void FirstStartWidget::setupUi()
{
    auto outerLayout = gsl::owner<QVBoxLayout*>(new QVBoxLayout(this));
    outerLayout->setAlignment(Qt::AlignCenter);
    _welcomeLabel = gsl::owner<QLabel*>(new QLabel);
    outerLayout->addWidget(_welcomeLabel);
    _descriptionLabel = gsl::owner<QLabel*>(new QLabel);
    outerLayout->addWidget(_descriptionLabel);

    _themeSelectorWidget = gsl::owner<ThemeSelectorWidget*>(new ThemeSelectorWidget(this));
    _generalSettingsWidget = gsl::owner<GeneralSettingsWidget*>(new GeneralSettingsWidget(this));

    outerLayout->addWidget(_generalSettingsWidget);
    outerLayout->addWidget(_themeSelectorWidget);

    _doneButton = gsl::owner<QPushButton*>(new QPushButton);
    connect(_doneButton, &QPushButton::clicked, this, &FirstStartWidget::dismissed);
    auto buttonBar = gsl::owner<QHBoxLayout*>(new QHBoxLayout);
    buttonBar->setAlignment(Qt::AlignRight);
    buttonBar->addWidget(_doneButton);
    outerLayout->addLayout(buttonBar);

    retranslateUi();
}

bool FirstStartWidget::eventFilter(QObject* object, QEvent* event)
{
    if (object == this && event->type() == QEvent::LanguageChange) {
        this->retranslateUi();
    }
    return QWidget::eventFilter(object, event);
}

void FirstStartWidget::retranslateUi()
{
    _doneButton->setText(tr("Done"));
    _welcomeLabel->setText(
        QLatin1String("<h1>") + tr("Welcome to %1").arg(QStringLiteral("Tungsten CAD"))
        + QLatin1String("</h1>")
    );
    _descriptionLabel->setText(
        tr("Set your modeling preferences below to start with a focused Fusion 360 style workspace.")
        + QLatin1String(" ")
        + tr("You can update these options at any time in preferences.")
    );
}
