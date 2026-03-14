#ifndef SANDBOX_DOCUMENTPROTECTOR_H
#define SANDBOX_DOCUMENTPROTECTOR_H

#include <string>
#include <App/DocumentObserver.h>

namespace App {
    class Document;
    class DocumentObject;
}

namespace Sandbox {

class SandboxAppExport DocumentProtector : public App::DocumentObserver
{
public:
    DocumentProtector(App::Document*);
    ~DocumentProtector();

    static void init();

    App::DocumentObject *addObject(const std::string& type, const std::string& name="");
    void removeObject(const std::string& name);
    void recompute();

private:
    /** Checks if the given document is about to be closed */
    void slotDeletedDocument(const App::Document& Doc);
    void validate();
};

class AbstractCallable
{
public:
    AbstractCallable()
    {
    }
    virtual ~AbstractCallable()
    {
    }

    virtual void operator()() const = 0;
};

template <class T, void (T::*method)()>
class Callable : public AbstractCallable
{
public:
    Callable(App::DocumentObject* o) : obj(o)
    {
    }
    virtual ~Callable()
    {
    }

    virtual void operator()() const
    {
        T* v = static_cast<T*>(obj);
        (v->*method)();
    }

private:
    App::DocumentObject* obj;
};

template <class T, class Arg, void (T::*method)(Arg)>
class CallableWithArgs : public AbstractCallable
{
public:
    CallableWithArgs(App::DocumentObject* o, Arg a) : obj(o), arg(a)
    {
    }
    virtual ~CallableWithArgs()
    {
    }

    virtual void operator()() const
    {
        T* v = static_cast<T*>(obj);
        (v->*method)(arg);
    }

private:
    App::DocumentObject* obj;
    Arg arg;
};

class SandboxAppExport DocumentObjectProtector
{
public:
    DocumentObjectProtector(App::DocumentObject*);
    ~DocumentObjectProtector();

    App::DocumentObject* getObject() const;
    bool setProperty(const std::string& name, const App::Property& p);
    void execute(const AbstractCallable&);
    void purgeTouched();

private:
    void validate();

private:
    App::DocumentObject* obj;
};

}

#endif // SANDBOX_DOCUMENTPROTECTOR_H

