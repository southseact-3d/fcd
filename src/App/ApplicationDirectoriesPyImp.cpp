#include <Python.h>
#include <filesystem>
#include <string>
#include <vector>


#include "Application.h"

// inclusion of the generated files (generated out of ApplicationDirectories.pyi)
#include <App/ApplicationDirectoriesPy.h>
#include <App/ApplicationDirectoriesPy.cpp>  // NOLINT

namespace fs = std::filesystem;

using namespace App;

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg)

// returns a string which represent the object e.g. when printed in python
std::string ApplicationDirectoriesPy::representation() const
{
    return {"<ApplicationDirectoriesPy object>"};
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::usingCurrentVersionConfig(PyObject* args)
{
    char *path = nullptr;
    if (!PyArg_ParseTuple(args, "s", &path)) {
        return nullptr;
    }
    bool result = App::Application::directories()->usingCurrentVersionConfig(Base::FileInfo::stringToPath(path));
    return Py::new_reference_to(Py::Boolean(result));
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::migrateAllPaths(PyObject* args)
{
    PyObject* object {nullptr};
    if (!PyArg_ParseTuple(args, "O", &object)) {
        return nullptr;
    }

    if (PyTuple_Check(object) || PyList_Check(object)) {
        Py::Sequence seq(object);
        Py::Sequence::size_type size = seq.size();
        std::vector<fs::path> paths;
        paths.resize(size);

        for (Py::Sequence::size_type i = 0; i < size; i++) {
            Py::Object item = seq[i];

            if (!PyUnicode_Check(item.ptr())) {
                PyErr_SetString(PyExc_RuntimeError, "path was not a string");
                return nullptr;
            }
            const char* s = PyUnicode_AsUTF8(item.ptr());
            if (!s) {
                return nullptr; // PyUnicode_AsUTF8 sets an error
            }
            paths[i] = Base::FileInfo::stringToPath(s);
        }
        App::Application::directories()->migrateAllPaths(paths);
    }
    Py_Return;
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::versionStringForPath(PyObject* args)
{
    int major {0};
    int minor {0};
    if (!PyArg_ParseTuple(args, "ii", &major, &minor)) {
        return nullptr;
    }
    auto result = App::ApplicationDirectories::versionStringForPath(major, minor);
    return Py::new_reference_to(Py::String(result));
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::isVersionedPath(PyObject* args)
{
    char *path = nullptr;
    if (!PyArg_ParseTuple(args, "s", &path)) {
        return nullptr;
    }
    bool result = App::Application::directories()->isVersionedPath(Base::FileInfo::stringToPath(path));
    return Py::new_reference_to(Py::Boolean(result));
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::mostRecentAvailableConfigVersion(PyObject* args)
{
    char *path = nullptr;
    if (!PyArg_ParseTuple(args, "s", &path)) {
        return nullptr;
    }
    std::string result = App::Application::directories()->mostRecentAvailableConfigVersion(Base::FileInfo::stringToPath(path));
    return Py::new_reference_to(Py::String(result));
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::mostRecentConfigFromBase(PyObject* args)
{
    char *path = nullptr;
    if (!PyArg_ParseTuple(args, "s", &path)) {
        return nullptr;
    }
    fs::path result = App::Application::directories()->mostRecentConfigFromBase(Base::FileInfo::stringToPath(path));
    return Py::new_reference_to(Py::String(Base::FileInfo::pathToString(result)));
}

[[maybe_unused]] PyObject* ApplicationDirectoriesPy::migrateConfig(PyObject* args)
{
    char *oldPath = nullptr;
    char *newPath = nullptr;
    if (!PyArg_ParseTuple(args, "ss", &oldPath, &newPath)) {
        return nullptr;
    }
    App::ApplicationDirectories::migrateConfig(
        Base::FileInfo::stringToPath(oldPath),
        Base::FileInfo::stringToPath(newPath));
    Py_Return;
}

PyObject* ApplicationDirectoriesPy::getCustomAttributes([[maybe_unused]] const char* attr) const
{
    return nullptr;
}

int ApplicationDirectoriesPy::setCustomAttributes([[maybe_unused]] const char* attr, [[maybe_unused]] PyObject* obj)
{
    return 0;
}

// NOLINTEND(cppcoreguidelines-pro-type-vararg)
