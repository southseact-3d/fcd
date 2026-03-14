#ifndef MESH_UTILITIES_H
#define MESH_UTILITIES_H

#include <Base/Converter.h>
#include <Mod/Mesh/App/WildMagic4/Wm4Vector3.h>


namespace Base
{
// Specialization for Wm4::Vector3d
template<>
struct vec_traits<Wm4::Vector3d>
{
    using vec_type = Wm4::Vector3d;
    using float_type = double;
    explicit vec_traits(const vec_type& v)
        : v(v)
    {}
    inline std::tuple<float_type, float_type, float_type> get() const
    {
        return std::make_tuple(v.X(), v.Y(), v.Z());
    }

private:
    const vec_type& v;
};
// Specialization for Wm4::Vector3f
template<>
struct vec_traits<Wm4::Vector3f>
{
    using vec_type = Wm4::Vector3f;
    using float_type = float;
    explicit vec_traits(const vec_type& v)
        : v(v)
    {}
    inline std::tuple<float_type, float_type, float_type> get() const
    {
        return std::make_tuple(v.X(), v.Y(), v.Z());
    }

private:
    const vec_type& v;
};
}  // namespace Base


#endif  // MESH_UTILITIES_H
