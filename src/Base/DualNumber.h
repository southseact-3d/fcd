#include <cmath>

// NOLINTBEGIN(readability-identifier-length)
namespace Base
{


/**
 * @brief Dual Numbers are 2-part numbers like complex numbers, but different
 * algebra. They are denoted as a + b*eps, where eps^2 = 0. eps, the nilpotent,
 * is like imaginary unit of complex numbers. The neat utility of dual numbers
 * is that if you use them instead of normal numbers in a function like sin(),
 * derivative is implicitly calculated as a multiplier to the dual part.
 */
class DualNumber
{
public:
    double re = 0.0;
    double du = 0.0;

public:
    DualNumber() = default;
    DualNumber(double re, double du = 0.0)  // NOLINT
        : re(re)
        , du(du)
    {}
    DualNumber operator-() const
    {
        return {-re, -du};
    }
};

inline DualNumber operator+(DualNumber a, DualNumber b)
{
    return {a.re + b.re, a.du + b.du};
}
inline DualNumber operator+(DualNumber a, double b)
{
    return {a.re + b, a.du};
}
inline DualNumber operator+(double a, DualNumber b)
{
    return {a + b.re, b.du};
}

inline DualNumber operator-(DualNumber a, DualNumber b)
{
    return {a.re - b.re, a.du - b.du};
}
inline DualNumber operator-(DualNumber a, double b)
{
    return {a.re - b, a.du};
}
inline DualNumber operator-(double a, DualNumber b)
{
    return {a - b.re, -b.du};
}

inline DualNumber operator*(DualNumber a, DualNumber b)
{
    return {a.re * b.re, a.re * b.du + a.du * b.re};
}
inline DualNumber operator*(double a, DualNumber b)
{
    return {a * b.re, a * b.du};
}
inline DualNumber operator*(DualNumber a, double b)
{
    return {a.re * b, a.du * b};
}

inline DualNumber operator/(DualNumber a, DualNumber b)
{
    return {a.re / b.re, (a.du * b.re - a.re * b.du) / (b.re * b.re)};
}
inline DualNumber operator/(DualNumber a, double b)
{
    return {a.re / b, a.du / b};
}

inline DualNumber pow(DualNumber a, double pw)
{
    return {std::pow(a.re, pw), pw * std::pow(a.re, pw - 1.0) * a.du};
}
}  // namespace Base
// NOLINTEND(readability-identifier-length)


#endif
