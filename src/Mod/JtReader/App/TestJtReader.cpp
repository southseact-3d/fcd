#include <vector>


#include <Base/Console.h>
#include <Base/FileInfo.h>

#include "FcLodHandler.h"
#include "TestJtReader.h"


TestJtReader::TestJtReader()
{}


TestJtReader::~TestJtReader()
{}

void TestJtReader::read(void)
{
    // const std::vector<TOC_Entry>& toc = readToc();

    for (std::vector<TOC_Entry>::const_iterator i = TocEntries.begin(); i != TocEntries.end(); ++i) {
        int segType = i->getSegmentType();

        if (segType == 7) {
            FcLodHandler handler;

            readLodSegment(*i, handler);
        }


        Base::Console().log(i->toString().c_str());
    }
}
