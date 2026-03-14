#ifndef GUI_METATYPES_H
#define GUI_METATYPES_H

#include <Base/Vector3D.h>
#include <Base/Matrix.h>
#include <Base/Placement.h>
#include <Base/Quantity.h>
#include <App/DocumentObserver.h>
#include <QMetaType>
#include <QList>

// NOLINTBEGIN
Q_DECLARE_METATYPE(Base::Vector3f)
Q_DECLARE_METATYPE(Base::Vector3d)
Q_DECLARE_METATYPE(QList<Base::Vector3d>)
Q_DECLARE_METATYPE(Base::Matrix4D)
Q_DECLARE_METATYPE(Base::Placement)
Q_DECLARE_METATYPE(Base::Rotation)
Q_DECLARE_METATYPE(Base::Quantity)
Q_DECLARE_METATYPE(QList<Base::Quantity>)
Q_DECLARE_METATYPE(App::SubObjectT)
Q_DECLARE_METATYPE(QList<App::SubObjectT>)
// NOLINTEND

#endif  // GUI_METATYPES_H
