#ifndef BASE_UNITSCONVDATA_H
#define BASE_UNITSCONVDATA_H

namespace Base::UnitsConvData
{

constexpr auto in {25.4};
constexpr auto ft {12 * in};
constexpr auto yd {3 * ft};
constexpr auto mi {1760 * yd};
constexpr auto lb {0.45359237};
constexpr auto lbf {9.80665 * lb};
constexpr auto psi {lbf / (in * in) * 1000};

}  // namespace Base::UnitsConvData

#endif
