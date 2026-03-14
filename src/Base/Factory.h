#ifndef BASE_FACTORY_H
#define BASE_FACTORY_H

#include <list>
#include <map>
#include <string>
#ifndef FC_GLOBAL_H
# include <FCGlobal.h>
#endif


namespace Base
{

/// Abstract base class of all producers
class BaseExport AbstractProducer
{
public:
    AbstractProducer() = default;
    virtual ~AbstractProducer() = default;
    /// overwritten by a concrete producer to produce the needed object
    virtual void* Produce() const = 0;

    FC_DISABLE_COPY_MOVE(AbstractProducer)
};


/** Base class of all factories
 * This class has the purpose to produce instances of classes at runtime
 * that are unknown at compile time. It holds a map of so called
 * producers which are able to produce an instance of a special class.
 * Producer can be registered at runtime through e.g. application modules
 */
class BaseExport Factory
{
public:
    /// Adds a new prducer instance
    void AddProducer(const char* sClassName, AbstractProducer* pcProducer);
    /// returns true if there is a producer for this class registered
    bool CanProduce(const char* sClassName) const;
    /// returns a list of all registered producer
    std::list<std::string> CanProduce() const;
    /// destruction
    virtual ~Factory();

    FC_DISABLE_COPY_MOVE(Factory)

protected:
    /// produce a class with the given name
    void* Produce(const char* sClassName) const;
    /// construction
    Factory() = default;

    std::map<const std::string, AbstractProducer*> _mpcProducers;
};

// --------------------------------------------------------------------

/** The ScriptFactorySingleton singleton
 */
class BaseExport ScriptFactorySingleton: public Factory  // NOLINT
{
public:
    static ScriptFactorySingleton& Instance();
    static void Destruct();

    const char* ProduceScript(const char* sScriptName) const;

    FC_DISABLE_COPY_MOVE(ScriptFactorySingleton)

private:
    static ScriptFactorySingleton* _pcSingleton;  // NOLINT

protected:
    ScriptFactorySingleton() = default;
    ~ScriptFactorySingleton() override = default;
};

inline ScriptFactorySingleton& ScriptFactory()
{
    return ScriptFactorySingleton::Instance();
}

// --------------------------------------------------------------------

/** Script Factory
 * This class produce Scripts.
 * @see Factory
 */
class BaseExport ScriptProducer: public AbstractProducer
{
public:
    /// Constructor
    ScriptProducer(const char* name, const char* script)
        : mScript(script)
    {
        ScriptFactorySingleton::Instance().AddProducer(name, this);
    }

    ~ScriptProducer() override = default;

    /// Produce an instance
    void* Produce() const override
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        return const_cast<char*>(mScript);
    }

    FC_DISABLE_COPY_MOVE(ScriptProducer)

private:
    const char* mScript;
};

}  // namespace Base


#endif
