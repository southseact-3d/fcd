#ifndef Fem_VTK_vtkFemFrameSourceAlgorithm_H
#define Fem_VTK_vtkFemFrameSourceAlgorithm_H

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridAlgorithm.h>

class vtkInformation;
class vtkInformationVector;


namespace Fem
{

// algorithm that allows multi frame handling: if data is stored in MultiBlock dataset
// this source enables the downstream filters to query the blocks as different time frames
class vtkFemFrameSourceAlgorithm: public vtkUnstructuredGridAlgorithm
{
public:
    static vtkFemFrameSourceAlgorithm* New();
    vtkTypeMacro(vtkFemFrameSourceAlgorithm, vtkUnstructuredGridAlgorithm);

    bool isValid();
    void setDataObject(vtkSmartPointer<vtkDataObject> data);
    std::vector<double> getFrameValues();

protected:
    vtkFemFrameSourceAlgorithm();
    ~vtkFemFrameSourceAlgorithm() override;

    vtkSmartPointer<vtkDataObject> m_data;

    int RequestInformation(
        vtkInformation* reqInfo,
        vtkInformationVector** inVector,
        vtkInformationVector* outVector
    ) override;
    int RequestData(
        vtkInformation* reqInfo,
        vtkInformationVector** inVector,
        vtkInformationVector* outVector
    ) override;
};

}  // namespace Fem


#endif  // Fem_VTK_vtkFemFrameSourceAlgorithm_H
