#ifndef PLANEGCS_UTIL_H
#define PLANEGCS_UTIL_H

#include <map>
#include <set>
#include <vector>


namespace GCS
{
using VEC_pD = std::vector<double*>;
using VEC_D = std::vector<double>;
using VEC_I = std::vector<int>;
using MAP_pD_pD = std::map<double*, double*>;
using MAP_pD_D = std::map<double*, double>;
using MAP_pD_I = std::map<double*, int>;
using SET_pD = std::set<double*>;
using SET_I = std::set<int>;
}  // namespace GCS

#endif  // PLANEGCS_UTIL_H
