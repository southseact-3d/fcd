#include <Base/GeometryPyCXX.h>
#include <Base/Interpreter.h>
#include <Base/VectorPy.h>
#include <Base/Tools.h>


namespace PartDesign
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("_PartDesign")
    {
        add_varargs_method("makeFilletArc", &Module::makeFilletArc, "makeFilletArc(...) -- Fillet arc.");
        initialize("This module is the PartDesign module.");  // register with Python
    }

private:
    Py::Object makeFilletArc(const Py::Tuple& args)
    {
        PyObject* pM1;
        PyObject* pP;
        PyObject* pQ;
        PyObject* pN;
        double r2;
        int ccw;
        if (!PyArg_ParseTuple(
                args.ptr(),
                "O!O!O!O!di",
                &(Base::VectorPy::Type),
                &pM1,
                &(Base::VectorPy::Type),
                &pP,
                &(Base::VectorPy::Type),
                &pQ,
                &(Base::VectorPy::Type),
                &pN,
                &r2,
                &ccw
            )) {
            throw Py::Exception();
        }

        Base::Vector3d M1 = Py::Vector(pM1, false).toVector();
        Base::Vector3d P = Py::Vector(pP, false).toVector();
        Base::Vector3d Q = Py::Vector(pQ, false).toVector();
        Base::Vector3d N = Py::Vector(pN, false).toVector();

        Base::Vector3d u = Q - P;
        Base::Vector3d v = P - M1;
        Base::Vector3d b;
        if (ccw) {
            b = u % N;
        }
        else {
            b = N % u;
        }
        b.Normalize();

        double uu = u * u;
        double uv = u * v;
        double r1 = v.Length();

        // distinguish between internal and external fillets
        r2 *= Base::sgn(uv);

        double cc = 2.0 * r2 * (b * v - r1);
        double d = uv * uv - uu * cc;
        if (d < 0) {
            throw Py::RuntimeError("Unable to calculate intersection points");
        }

        double t;
        double t1 = (-uv + sqrt(d)) / uu;
        double t2 = (-uv - sqrt(d)) / uu;

        if (fabs(t1) < fabs(t2)) {
            t = t1;
        }
        else {
            t = t2;
        }

        Base::Vector3d M2 = P + (u * t) + (b * r2);
        Base::Vector3d S1 = (r2 * M1 + r1 * M2) / (r1 + r2);
        Base::Vector3d S2 = M2 - (b * r2);

        Py::Tuple tuple(3);
        tuple.setItem(0, Py::Vector(S1));
        tuple.setItem(1, Py::Vector(S2));
        tuple.setItem(2, Py::Vector(M2));

        return tuple;
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace PartDesign
