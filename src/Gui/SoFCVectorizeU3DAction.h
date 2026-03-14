#ifndef GUI_SOFCVECTORIZEU3DACTION_H
#define GUI_SOFCVECTORIZEU3DACTION_H

#include <fstream>

#include <Inventor/annex/HardCopy/SoVectorizeAction.h>
#include <Inventor/annex/HardCopy/SoVectorOutput.h>

#include <FCGlobal.h>

namespace Gui
{

class GuiExport SoU3DVectorOutput: public SoVectorOutput
{
public:
    SoU3DVectorOutput();
    ~SoU3DVectorOutput() override;

    SbBool openFile(const char* filename) override;
    void closeFile() override;
    std::fstream& getFileStream();

private:
    std::fstream file;
};

/**
 * @author Werner Mayer
 */
class SoFCVectorizeU3DActionP;
class GuiExport SoFCVectorizeU3DAction: public SoVectorizeAction
{
    using inherited = SoVectorizeAction;

    SO_ACTION_HEADER(SoFCVectorizeU3DAction);

public:
    SoFCVectorizeU3DAction();
    ~SoFCVectorizeU3DAction() override;

    static void initClass();
    SoU3DVectorOutput* getU3DOutput() const;

protected:
    void beginTraversal(SoNode* node) override;
    void endTraversal(SoNode* node) override;
    void printHeader() const override;
    void printFooter() const override;
    void printBackground() const override;
    void printItem(const SoVectorizeItem* item) const override;
    void printViewport() const override;

private:
    static void actionMethod(SoAction*, SoNode*);

private:
    SoFCVectorizeU3DActionP* p;
    friend class SoFCVectorizeU3DActionP;
};

}  // namespace Gui

#endif  // GUI_SOFCVECTORIZEU3DACTION_H
