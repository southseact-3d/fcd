#ifndef GUI_PYTHONWRAPPER_H
#define GUI_PYTHONWRAPPER_H

#include "qglobal.h"
#include <Base/PyObjectBase.h>
#include <FCGlobal.h>

QT_BEGIN_NAMESPACE
class QAction;
class QDir;
class QIcon;
class QImage;
class QGraphicsObject;
class QGraphicsItem;
class QObject;
class QPrinter;
class QWidget;
QT_END_NAMESPACE

namespace Gui
{

class GuiExport PythonWrapper
{
public:
    PythonWrapper();
    bool loadCoreModule();
    bool loadGuiModule();
    bool loadWidgetsModule();
    bool loadPrintSupportModule();
    bool loadUiToolsModule();

    bool toCString(const Py::Object&, std::string&);
    QObject* toQObject(const Py::Object&);
    qsizetype toEnum(PyObject* pyPtr);
    qsizetype toEnum(const Py::Object& pyobject);
    Py::Object toStandardButton(qsizetype);
    QGraphicsItem* toQGraphicsItem(PyObject* ptr);
    QGraphicsItem* toQGraphicsItem(const Py::Object& pyObject);
    QGraphicsObject* toQGraphicsObject(PyObject* pyPtr);
    QGraphicsObject* toQGraphicsObject(const Py::Object& pyObject);

    Py::Object fromQAction(QAction*);
    Py::Object fromQPrinter(QPrinter*);
    Py::Object fromQObject(QObject*, const char* className = nullptr);
    Py::Object fromQWidget(QWidget*, const char* className = nullptr);
    const char* getWrapperName(QObject*) const;

    Py::Object fromQImage(const QImage&);
    QImage* toQImage(PyObject* pyobj);
    /*!
      Create a Python wrapper for the icon. The icon must be created on the heap
      and the Python wrapper takes ownership of it.
     */
    Py::Object fromQIcon(const QIcon*);
    QIcon* toQIcon(PyObject* pyobj);
    Py::Object fromQDir(const QDir&);
    QDir* toQDir(PyObject* pyobj);
    static void createChildrenNameAttributes(PyObject* root, QObject* object);
    static void setParent(PyObject* pyWdg, QObject* parent);

private:
    qsizetype tryEnum(PyObject* pyPtr);
    Py::Object tryToStandardButton(qsizetype value);
};

}  // namespace Gui

#endif  // GUI_PYTHONWRAPPER_H
