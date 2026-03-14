#ifndef _ImportStep_h_
#define _ImportStep_h_

#include <Mod/Part/PartGlobal.h>

namespace App
{
class Document;
}

namespace Part
{

/** The part shape property
 */
PartExport int ImportStepParts(App::Document* pcDoc, const char* Name);

inline std::list<std::string> supportedSTEPSchemes()
{
    std::list<std::string> schemes;
    schemes.emplace_back("AP203");
    schemes.emplace_back("AP214CD");
    schemes.emplace_back("AP214DIS");
    schemes.emplace_back("AP214IS");
    schemes.emplace_back("AP242DIS");
    return schemes;
}

}  // namespace Part


#endif
