// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef PARTDESIGN_TAB_BAR_H
#define PARTDESIGN_TAB_BAR_H

#include <QWidget>
#include <QTabBar>
#include <QToolButton>
#include <QHBoxLayout>
#include <QIcon>

namespace PartDesignGui
{

class PartDesignTabBar: public QWidget
{
    Q_OBJECT

public:
    explicit PartDesignTabBar(QWidget* parent = nullptr);
    ~PartDesignTabBar() override;

    void setCurrentIndex(int index);
    int currentIndex() const;

Q_SIGNALS:
    void currentIndexChanged(int index);

private:
    void setupUI();
    void onTabClicked(int index);

    QTabBar* tabBar;
    QHBoxLayout* layout;
};

}  // namespace PartDesignGui

#endif  // PARTDESIGN_TAB_BAR_H
