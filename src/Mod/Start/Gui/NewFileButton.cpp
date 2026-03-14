#include <QLabel>
#include <QFont>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

#include "NewFileButton.h"
#include <algorithm>

namespace StartGui
{

NewFileButton::NewFileButton(const NewButton& newButton)
    : mainLayout(new QHBoxLayout(this))
    , textLayout(new QVBoxLayout())
    , headingLabel(new QLabel())
{
    setObjectName(QStringLiteral("newFileButton"));
    auto hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Start"
    );

    constexpr int defaultWidth = 180;  // #newFileButton width in QSS
    labelWidth = int(hGrp->GetInt("FileCardLabelWith", defaultWidth));

    constexpr int defaultSize = 48;
    iconSize = int(hGrp->GetInt("NewFileIconSize", defaultSize));

    auto iconLabel = new QLabel(this);
    QIcon baseIcon(newButton.iconPath);
    iconLabel->setPixmap(baseIcon.pixmap(iconSize, iconSize));
    iconLabel->setAlignment(Qt::AlignHCenter);

    textLayout->addWidget(headingLabel);
    textLayout->setSpacing(0);
    textLayout->setContentsMargins(0, 0, 0, 0);

    headingLabel->setText(newButton.heading);
    QFont font = headingLabel->font();
    font.setWeight(QFont::Bold);
    headingLabel->setFont(font);

    mainLayout->setAlignment(Qt::AlignCenter);
    textLayout->insertWidget(0, iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addLayout(textLayout);
    mainLayout->addStretch();
    QFontMetrics qfm(font);
    int margin = qfm.height() / 2;
    mainLayout->setSpacing(margin);
    mainLayout->setContentsMargins(margin, margin, 2 * margin, margin);
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setToolTip(QStringLiteral("<b>%1</b><br/>%2").arg(newButton.heading, newButton.description));
}

QSize NewFileButton::minimumSizeHint() const
{
    int minWidth = labelWidth + iconSize + mainLayout->contentsMargins().left()
        + mainLayout->contentsMargins().right() + mainLayout->spacing();

    int textHeight = headingLabel->sizeHint().height();

    int minHeight = std::max(iconSize, textHeight) + mainLayout->contentsMargins().top()
        + mainLayout->contentsMargins().bottom();

    return {minWidth, minHeight};
}

}  // namespace StartGui
