#ifndef FEM_PROPERTYPOSTDATASET_H
#define FEM_PROPERTYPOSTDATASET_H

#include <vtkDataObject.h>
#include <vtkSmartPointer.h>

#include <App/Property.h>
#include <Mod/Fem/FemGlobal.h>


namespace Fem
{

/** The vtk data set property class.
 * @author Stefan Tröger
 */
class FemExport PropertyPostDataObject: public App::Property
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyPostDataObject();
    ~PropertyPostDataObject() override;

    /** @name Getter/setter */
    //@{
    /// Scale the point coordinates of the data set with factor \a s
    void scale(double s);
    /// set the dataset
    void setValue(const vtkSmartPointer<vtkDataObject>&);
    /// get the part shape
    const vtkSmartPointer<vtkDataObject>& getValue() const;
    /// check if we hold a dataset or a dataobject (which would mean a composite data structure)
    bool isDataSet();
    bool isComposite();
    int getDataType();
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject() override;
    void setPyObject(PyObject* value) override;
    //@}

    /** @name Save/restore */
    //@{
    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;

    void SaveDocFile(Base::Writer& writer) const override;
    void RestoreDocFile(Base::Reader& reader) override;

    App::Property* Copy() const override;
    void Paste(const App::Property& from) override;
    unsigned int getMemSize() const override;
    //@}

    /// Get valid paths for this property; used by auto completer
    void getPaths(std::vector<App::ObjectIdentifier>& paths) const override;

private:
    static void scaleDataObject(vtkDataObject*, double s);

protected:
    void createDataObjectByExternalType(vtkSmartPointer<vtkDataObject> ex);
    vtkSmartPointer<vtkDataObject> m_dataObject;
};

}  // namespace Fem


#endif  // FEM_PROPERTYPOSTDATASET_H
