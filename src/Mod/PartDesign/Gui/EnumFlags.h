#ifndef PARTDESIGNGUI_ENUMFLAGS_H
#define PARTDESIGNGUI_ENUMFLAGS_H

namespace PartDesignGui
{

// https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/
// https://www.boost.org/doc/libs/1_66_0/boost/detail/bitmask.hpp
// https://stackoverflow.com/questions/1448396/how-to-use-enums-as-flags-in-c

enum class AllowSelection
{
    NONE = 0,           /**< This is used to indicate to stop the selection */
    EDGE = 1 << 0,      /**< Allow picking edges */
    FACE = 1 << 1,      /**< Allow picking faces */
    PLANAR = 1 << 2,    /**< Allow only linear edges and planar faces */
    CIRCLE = 1 << 3,    /**< Allow picking circular edges (incl arcs) */
    POINT = 1 << 4,     /**< Allow picking datum points */
    OTHERBODY = 1 << 5, /**< Allow picking objects from another body in the same part */
    WHOLE = 1 << 6      /**< Allow whole object selection */
};
Q_DECLARE_FLAGS(AllowSelectionFlags, AllowSelection)

}  // namespace PartDesignGui

Q_DECLARE_OPERATORS_FOR_FLAGS(PartDesignGui::AllowSelectionFlags)

#endif  // PARTDESIGNGUI_ENUMFLAGS_H
