#ifndef MESHGUI_REMESHGMSH_H
#define MESHGUI_REMESHGMSH_H

#include <Mod/Mesh/MeshGlobal.h>

#include <memory>
#include <QDialog>
#include <QProcess>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>


namespace Mesh
{
class Feature;
}

namespace Gui
{
class StatusWidget;
}

namespace MeshGui
{

/**
 * Non-modal dialog to remesh an existing mesh.
 * @author Werner Mayer
 */
class MeshGuiExport GmshWidget: public QWidget
{
    Q_OBJECT

public:
    explicit GmshWidget(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~GmshWidget() override;
    void accept();
    void reject();

protected:
    void changeEvent(QEvent* e) override;
    int meshingAlgorithm() const;
    double getAngle() const;
    double getMaxSize() const;
    double getMinSize() const;
    virtual bool writeProject(QString& inpFile, QString& outFile);
    virtual bool loadOutput();

private:
    void setupConnections();
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void errorOccurred(QProcess::ProcessError error);
    void onKillButtonClicked();
    void onClearButtonClicked();

    void readyReadStandardError();
    void readyReadStandardOutput();

private:
    class Private;
    std::unique_ptr<Private> d;

    Q_DISABLE_COPY_MOVE(GmshWidget)
};

/**
 * Non-modal dialog to remesh an existing mesh.
 * @author Werner Mayer
 */
class MeshGuiExport RemeshGmsh: public GmshWidget
{
    Q_OBJECT

public:
    explicit RemeshGmsh(
        Mesh::Feature* mesh,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~RemeshGmsh() override;

protected:
    bool writeProject(QString& inpFile, QString& outFile) override;
    bool loadOutput() override;

private:
    class Private;
    std::unique_ptr<Private> d;

    Q_DISABLE_COPY_MOVE(RemeshGmsh)
};

/**
 * Embed the panel into a task dialog.
 */
class TaskRemeshGmsh: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskRemeshGmsh(Mesh::Feature* mesh);

public:
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Apply | QDialogButtonBox::Close;
    }
    bool isAllowedAlterDocument() const override
    {
        return true;
    }

private:
    RemeshGmsh* widget;
};

}  // namespace MeshGui

#endif  // MESHGUI_REMESHGMSH_H
