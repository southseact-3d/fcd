#ifndef FCREPALGOAPIBOOLEANOPERATION_H
#define FCREPALGOAPIBOOLEANOPERATION_H

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <Message_ProgressRange.hxx>

class FCBRepAlgoAPIHelper
{
public:
    static void setAutoFuzzy(BRepAlgoAPI_BooleanOperation* op);
    static void setAutoFuzzy(BRepAlgoAPI_BuilderAlgo* op);
};

class FCBRepAlgoAPI_BooleanOperation: public BRepAlgoAPI_BooleanOperation
{
public:
    DEFINE_STANDARD_ALLOC

    //! Empty constructor
    Standard_EXPORT FCBRepAlgoAPI_BooleanOperation();

    // set fuzzyness based on size
    void setAutoFuzzy();

    // not an override - real Build() has optionals, sadly type of those optionals that are differs
    // between OCCT versions
    Standard_EXPORT virtual void Build();  // NOLINT(clang-diagnostic-overloaded-virtual,
                                           // -Woverloaded-virtual)

#if OCC_VERSION_HEX >= 0x070600
    Standard_EXPORT void Build(const Message_ProgressRange& progressRange) Standard_OVERRIDE;
#else
    Standard_EXPORT virtual void Build(const Message_ProgressRange& progressRange);
#endif

protected:  //! @name Constructors
    //! Constructor to perform Boolean operation on only two arguments.
    //! Obsolete
    Standard_EXPORT FCBRepAlgoAPI_BooleanOperation(
        const TopoDS_Shape& theS1,
        const TopoDS_Shape& theS2,
        BOPAlgo_Operation theOperation
    );

private:
    Standard_EXPORT void RecursiveAddTools(const TopoDS_Shape& theTool);
    Standard_EXPORT void RecursiveCutFusedTools(
        const TopTools_ListOfShape& theOriginalArguments,
        const TopoDS_Shape& theTool
    );
    Standard_EXPORT void RecursiveCutCompound(const TopoDS_Shape& theArgument);
};
#endif
