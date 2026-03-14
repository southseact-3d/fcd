#ifndef PART_IMPORT_IGES_H
#define PART_IMPORT_IGES_H

#include <Mod/Part/PartGlobal.h>


namespace App
{
class Document;
}

namespace Part
{

PartExport int ImportIgesParts(App::Document* pcDoc, const char* Name);

}  // namespace Part

#endif  // PART_IMPORT_IGES_H
