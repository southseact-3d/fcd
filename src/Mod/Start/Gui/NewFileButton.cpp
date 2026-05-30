// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 Alfredo Monclus <alfredomonclus@gmail.com>          *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

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

    constexpr int defaultWidth = 200;
    labelWidth = int(hGrp->GetInt("FileCardLabelWith", defaultWidth));

    constexpr int defaultSize = 32;
    iconSize = int(hGrp->GetInt("NewFileIconSize", defaultSize));

    // Icon inside a grey rounded box (matches Option C .icon style)
    auto iconWrapper = new QLabel(this);
    iconWrapper->setFixedSize(32, 32);
    iconWrapper->setStyleSheet(
        QStringLiteral(
            "background: #f3f4f6; border-radius: 8px; padding: 0px;"
        )
    );
    iconWrapper->setAlignment(Qt::AlignCenter);

    auto iconLabel = new QLabel(iconWrapper);
    QIcon baseIcon(newButton.iconPath);
    iconLabel->setPixmap(baseIcon.pixmap(iconSize, iconSize));
    iconLabel->setAlignment(Qt::AlignCenter);

    // Overlay the pixmap centered in the wrapper
    auto iconOverlayLayout = new QHBoxLayout(iconWrapper);
    iconOverlayLayout->setContentsMargins(0, 0, 0, 0);
    iconOverlayLayout->addWidget(iconLabel, 0, Qt::AlignCenter);

    headingLabel->setText(newButton.heading);
    QFont font = headingLabel->font();
    font.setWeight(QFont::Medium);
    headingLabel->setFont(font);

    // Remove the old vertical textLayout structure — use a flat horizontal layout
    textLayout->setSpacing(0);
    textLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainLayout->addWidget(iconWrapper, 0, Qt::AlignVCenter);
    mainLayout->addWidget(headingLabel, 0, Qt::AlignVCenter);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(16, 10, 20, 10);
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setToolTip(QStringLiteral("<b>%1</b><br/>%2").arg(newButton.heading, newButton.description));
    setMinimumHeight(44);
}

QSize NewFileButton::minimumSizeHint() const
{
    int textWidth = headingLabel->fontMetrics().horizontalAdvance(headingLabel->text());
    int minWidth = 32 + textWidth + mainLayout->spacing()
        + mainLayout->contentsMargins().left() + mainLayout->contentsMargins().right();

    int minHeight = mainLayout->contentsMargins().top() + mainLayout->contentsMargins().bottom()
        + std::max(32, headingLabel->sizeHint().height());

    return {minWidth, minHeight};
}

}  // namespace StartGui
