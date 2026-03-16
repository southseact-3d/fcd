// SPDX-License-Identifier: LGPL-2.1-or-later

#include "PartDesignTabBar.h"
#include <QTabBar>
#include <QHBoxLayout>
#include <QIcon>
#include <QSizePolicy>

namespace PartDesignGui
{

PartDesignTabBar::PartDesignTabBar(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

PartDesignTabBar::~PartDesignTabBar() = default;

void PartDesignTabBar::setupUI()
{
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    tabBar = new QTabBar(this);
    tabBar->setDrawBase(false);
    tabBar->setExpanding(false);
    tabBar->setDocumentMode(true);
    tabBar->setMovable(false);
    tabBar->setUsesScrollButtons(false);

    QIcon designIcon;
    designIcon.addFile(QStringLiteral(":/icons/PartDesign_Workbench.svg"), QSize(), QIcon::Normal, QIcon::Off);
    tabBar->addTab(designIcon, tr("Design"));

    QIcon meshIcon;
    meshIcon.addFile(QStringLiteral(":/icons/Mesh_Workbench.svg"), QSize(), QIcon::Normal, QIcon::Off);
    tabBar->addTab(meshIcon, tr("Mesh"));

    tabBar->setIconSize(QSize(24, 24));
    tabBar->setTabToolTip(0, tr("Part Design tools for creating solid geometry"));
    tabBar->setTabToolTip(1, tr("Mesh tools for working with mesh objects"));

    connect(tabBar, &QTabBar::currentChanged, this, &PartDesignTabBar::onTabClicked);

    layout->addWidget(tabBar);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(32);
}

void PartDesignTabBar::onTabClicked(int index)
{
    Q_EMIT currentIndexChanged(index);
}

void PartDesignTabBar::setCurrentIndex(int index)
{
    if (tabBar) {
        tabBar->setCurrentIndex(index);
    }
}

int PartDesignTabBar::currentIndex() const
{
    return tabBar ? tabBar->currentIndex() : 0;
}

}  // namespace PartDesignGui
