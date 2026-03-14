#ifndef PROGRAMOPTIONSUTILITIES_H
#define PROGRAMOPTIONSUTILITIES_H

#include <FCConfig.h>

#include <algorithm>
#include <array>
#include <string>

/**
 * @brief A namespace for utilities.
 *
 * Currently, this namespace contains functionality to define custom syntax to
 * parse command line options.
 */
namespace App::Util
{

std::pair<std::string, std::string> customSyntax(std::string_view strIn)
{
    if (strIn.size() < 2) {
        return {};
    }

    char leadChr {strIn[0]};
    std::string rest {strIn.substr(1)};

    if (leadChr == '@') {
        return {"response-file", rest};
    }

    if (leadChr != '-') {
        return {};
    }

#if defined(FC_OS_MACOSX)
    if (rest.find("psn_") == 0) {
        return {"psn", rest.substr(4)};
    }
#endif

    if (rest == "widgetcount") {
        return {rest, ""};
    }

    constexpr std::array knowns {"display",
                                 "style",
                                 "graphicssystem",
                                 "geometry",
                                 "font",
                                 "fn",
                                 "background",
                                 "bg",
                                 "foreground",
                                 "fg",
                                 "button",
                                 "btn",
                                 "name",
                                 "title",
                                 "visual"};

    if (std::ranges::find(knowns, rest) != knowns.end()) {
        return {rest, "null"};
    }
    return {};
}

}  // namespace App::Util
#endif  // PROGRAMOPTIONSUTILITIES_H
