#ifndef SANDBOX_DOCUMENTTHREAD_H
#define SANDBOX_DOCUMENTTHREAD_H

#include <QThread>
#include <Base/Mutex.h>
#include <Base/Handle.h>
#include <App/DocumentObject.h>

namespace Mesh {
    class MeshObject;
}

namespace Sandbox {

class SandboxAppExport DocumentThread : public QThread
{
public:
    DocumentThread(QObject* parent=0);
    ~DocumentThread();

protected:
    void run();
};

class SandboxAppExport WorkerThread : public QThread
{
public:
    WorkerThread(QObject* parent=0);
    ~WorkerThread();

protected:
    void run();
};

class SandboxAppExport PythonThread : public QThread
{
public:
    PythonThread(QObject* parent=0);
    ~PythonThread();

protected:
    void run();
    static QRecursiveMutex mutex;
};

class SandboxAppExport MeshLoaderThread : public QThread
{
public:
    MeshLoaderThread(const QString&, QObject* parent=0);
    ~MeshLoaderThread();

    Base::Reference<Mesh::MeshObject> getMesh() const;

protected:
    void run();

private:
    QString filename;
    Base::Reference<Mesh::MeshObject> mesh;
};

class SandboxObject : public App::DocumentObject
{
    PROPERTY_HEADER(SandboxObject);

public:
    SandboxObject();
    ~SandboxObject();

    App::PropertyInteger Integer;

    short mustExecute(void) const;
    App::DocumentObjectExecReturn *execute(void);
    void onChanged(const App::Property* prop);
    const char* getViewProviderName(void) const {
        return "Gui::ViewProviderDocumentObject";
    }

    void setIntValue(int);
    void resetValue();
};

class SandboxAppExport DocumentTestThread : public QThread
{
public:
    DocumentTestThread(QObject* parent=0);
    ~DocumentTestThread();

protected:
    void run();
};

class SandboxAppExport DocumentSaverThread : public QThread
{
public:
    DocumentSaverThread(App::Document* doc, QObject* parent=0);
    ~DocumentSaverThread();

protected:
    void run();

private:
    App::Document* doc;
};

}

#endif // SANDBOX_DOCUMENTTHREAD_H

