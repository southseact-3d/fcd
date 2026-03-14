#include <cassert>

#include "BaseClass.h"
#include "PyObjectBase.h"

using namespace Base;

Type BaseClass::classTypeId = Base::Type::BadType;


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
BaseClass::BaseClass() = default;

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
BaseClass::~BaseClass() = default;


//**************************************************************************
// separator for other implementation aspects

void BaseClass::init()
{
    assert(BaseClass::classTypeId.isBad() && "don't init() twice!");
    /* Make sure superclass gets initialized before subclass. */
    /*assert(strcmp(#_parentclass_), "inherited"));*/
    /*Type parentType(Type::fromName(#_parentclass_));*/
    /*assert(!parentType.isBad() && "you forgot init() on parentclass!");*/

    /* Set up entry in the type system. */
    BaseClass::classTypeId = Type::createType(Type::BadType, "Base::BaseClass", BaseClass::create);
}

Type BaseClass::getClassTypeId()
{
    return BaseClass::classTypeId;
}

Type BaseClass::getTypeId() const
{
    return BaseClass::classTypeId;
}


void BaseClass::initSubclass(
    Base::Type& toInit,
    const char* ClassName,
    const char* ParentName,
    Type::instantiationMethod method
)
{
    // don't init twice!
    assert(toInit.isBad());
    // get the parent class
    Base::Type parentType(Base::Type::fromName(ParentName));
    // forgot init parent!
    assert(!parentType.isBad());

    // create the new type
    toInit = Base::Type::createType(parentType, ClassName, method);
}

/**
 * This method returns the Python wrapper for a C++ object. It's in the responsibility of
 * the programmer to do the correct reference counting. Basically there are two ways how
 * to implement that: Either always return a new Python object then reference counting is
 * not a matter or return always the same Python object then the reference counter must be
 * incremented by one. However, it's absolutely forbidden to return always the same Python
 * object without incrementing the reference counter.
 *
 * The default implementation returns 'None'.
 */
PyObject* BaseClass::getPyObject()
{
    Py_Return;
}

void BaseClass::setPyObject(PyObject* /*unused*/)
{}
