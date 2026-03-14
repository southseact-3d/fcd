#ifndef GUI_MODULE_IO_H
#define GUI_MODULE_IO_H

#include <QCoreApplication>
#include <FCGlobal.h>

namespace Gui
{

class GuiExport ModuleIO
{
    Q_DECLARE_TR_FUNCTIONS(Gui::ModuleIO)

public:
    /*!
     * \brief verifyFile
     * Verifies the existence of the file. If it doesn't exist an error dialog
     * pops up and false is returned, otherwise true is returned.
     * \param filename
     * \return
     */
    static bool verifyFile(const QString& filename);
    /*!
     * \brief openFile
     * Opens the file.
     * The handling module is supposed to create a new document.
     * \param filename
     */
    static void openFile(const QString& filename);
    /*!
     * \brief verifyAndOpenFile
     * Verifies the existence of the file and opens it.
     * The handling module is supposed to create a new document.
     * \param filename
     */
    static void verifyAndOpenFile(const QString& filename);
    /*!
     * \brief importFile
     * Imports the files into the given document.
     * The handling module is supposed to create a new document if the passed
     * document doesn't exist.
     * \param filename
     * \param document
     */
    static void importFiles(const QStringList& filenames, const char* document);
};

}  // namespace Gui

#endif  // GUI_MODULE_IO_H
