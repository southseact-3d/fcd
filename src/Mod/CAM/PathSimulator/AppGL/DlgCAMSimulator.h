#ifndef PATHSIMULATOR_CAMSimulatorGui_H
#define PATHSIMULATOR_CAMSimulatorGui_H

#ifdef _MSC_VER
# pragma warning(disable : 4251)
#endif

#include <queue>
#include <functional>

#include <Mod/Part/App/TopoShape.h>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QPainter>
#include <QTimer>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QOpenGLContext>

namespace MillSim
{
// use short declaration as using 'include' causes a header loop
class MillSimulation;
class MillSimulationState;
struct Vertex;
}  // namespace MillSim

namespace Gui
{
class MDIView;
}

namespace CAMSimulator
{

class ViewCAMSimulator;

struct SimShape
{
public:
    std::vector<MillSim::Vertex> verts;
    std::vector<GLushort> indices;
    bool needsUpdate = false;
};

struct SimTool
{
public:
    std::vector<float> profile;
    int id;
    float diameter;
    float resolution;
};

class DlgCAMSimulator: public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit DlgCAMSimulator(ViewCAMSimulator& view, QWidget* parent = nullptr);
    ~DlgCAMSimulator() override;

    void cloneFrom(const DlgCAMSimulator& from);

    static DlgCAMSimulator* instance();

    void setAnimating(bool animating);
    void startSimulation(const Part::TopoShape& stock, float quality);
    void resetSimulation();

    void addGcodeCommand(const char* cmd);
    void addTool(
        const std::vector<float>& toolProfilePoints,
        int toolNumber,
        float diameter,
        float resolution
    );

    void setStockShape(const Part::TopoShape& tshape, float resolution);
    void setBaseShape(const Part::TopoShape& tshape, float resolution);

protected:
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;

    void updateResources();
    void updateWindowScale();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    bool mNeedsInitialize = false;
    bool mNeedsClear = false;
    bool mAnimating = false;
    QTimer mAnimatingTimer;

    std::unique_ptr<MillSim::MillSimulation> mMillSimulator;
    float mQuality = 10;

    std::vector<std::string> mGCode;
    std::size_t mLastGCode = 0;

    std::vector<SimTool> mTools;

    SimShape mStock;
    SimShape mBase;

    ViewCAMSimulator& mView;

    std::unique_ptr<MillSim::MillSimulationState> mState;
};

}  // namespace CAMSimulator

#endif  // PATHSIMULATOR_PathSim_H
