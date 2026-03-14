#ifndef WORKFLOWMANAGER_H_PB7A5GCM
#define WORKFLOWMANAGER_H_PB7A5GCM

namespace App
{
class Document;
}

namespace PartDesignGui
{

/**
 * Defines allowded tool set provided by the workbench
 * Legacy mode provides a free PartDesign features but forbids bodies and parts
 */
enum class Workflow
{
    Undetermined = 0,  ///< No workflow was chosen yet
    Legacy = 1 << 0,   ///< Old-style workflow with free features and no bodies
    Modern = 1 << 1,   ///< New-style workflow with bodies, parts etc
};

/**
 * This class controls the workflow of each file.
 * It has been introduced to support legacy files migrating to the new workflow.
 */
class PartDesignGuiExport WorkflowManager
{
public:
    virtual ~WorkflowManager();

    /**
     * Lookup the workflow of the document in the map.
     * If the document not in the map yet return Workflow::Undetermined.
     */

    Workflow getWorkflowForDocument(App::Document* doc);

    /**
     * Asserts the workflow of the document to be determined and prompt user to migrate if it is not
     * modern.
     *
     * If workflow was already chosen return it.
     * If the guesed workflow is Workflow::Legacy or Workflow::Mixed the user will be prompted to
     * migrate. If the user agrees the file will be migrated and the workflow will be set as modern.
     * If the user refuses to migrate use the old workflow.
     */
    Workflow determineWorkflow(App::Document* doc);

    /**
     * Force the desired workflow in document
     */
    void forceWorkflow(const App::Document* doc, Workflow wf);

    /** @name Init, Destruct an Access methods */
    //@{
    /// Creates an instance of the manager, should be called before any instance()
    static void init();
    /// Return an instance of the WorkflofManager.
    static WorkflowManager* instance();
    /// destroy the manager
    static void destruct();
    //@}

private:
    /// The class is not intended to be constructed outside of itself
    WorkflowManager();
    /// Get the signal on New document created
    void slotNewDocument(const App::Document& doc);
    /// Get the signal on document getting loaded
    void slotFinishRestoreDocument(const App::Document& doc);
    /// Get the signal on document close and remove it from our list
    void slotDeleteDocument(const App::Document& doc);

    /// Guess the Workflow of the document out of it's content
    Workflow guessWorkflow(const App::Document* doc);

private:
    std::map<const App::Document*, Workflow> dwMap;

    fastsignals::connection connectNewDocument;
    fastsignals::connection connectFinishRestoreDocument;
    fastsignals::connection connectDeleteDocument;

    static WorkflowManager* _instance;
};

/// Assures that workflow of the given document is determined and returns true if it is Workflow::Legacy
inline bool assureLegacyWorkflow(App::Document* doc)
{
    return WorkflowManager::instance()->determineWorkflow(doc) == Workflow::Legacy;
}

/// Assures that workflow of the given document is determined and returns true if it is Workflow::Modern
inline bool assureModernWorkflow(App::Document* doc)
{
    return WorkflowManager::instance()->determineWorkflow(doc) == Workflow::Modern;
}

/// Returns true if the workflow of the given document is Workflow::Legacy
inline bool isLegacyWorkflow(App::Document* doc)
{
    return WorkflowManager::instance()->getWorkflowForDocument(doc) == Workflow::Legacy;
}

/// Returns true if the workflow of the given document is Workflow::Modern
inline bool isModernWorkflow(App::Document* doc)
{
    return WorkflowManager::instance()->getWorkflowForDocument(doc) == Workflow::Modern;
}

}  // namespace PartDesignGui

#endif /* end of include guard: WORKFLOWMANAGER_H_PB7A5GCM */
