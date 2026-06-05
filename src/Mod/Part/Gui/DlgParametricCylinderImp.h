// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef PARTGUI_DLGPARTPARAMETRICCYLINDERIMP_H
#define PARTGUI_DLGPARTPARAMETRICCYLINDERIMP_H

#include <QDialog>

namespace PartGui
{
class Ui_DlgParametricCylinder;
using Ui_DlgParametricCylinderPtr = std::shared_ptr<Ui_DlgParametricCylinder>;

class DlgParametricCylinderImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgParametricCylinderImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgParametricCylinderImp() override;

    double getRadius() const;
    double getHeight() const;
    double getAngle() const;

private:
    Ui_DlgParametricCylinderPtr getUi() const;
    Ui_DlgParametricCylinderPtr ui;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGPARTPARAMETRICCYLINDERIMP_H
