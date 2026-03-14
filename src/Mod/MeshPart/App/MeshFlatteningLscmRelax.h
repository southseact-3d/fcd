// LeastSquareConformalMapping + fem relaxing
// ------------------------------------------
//
#ifndef UNWRAP_H
#define UNWRAP_H

// 1: local coordinates 2d representation  q_l_0
// 2: least square conformal map -> flat_vertices_0
// 3: local coordinates of mapped mesh q_l_1
// 4: diff in local coordinates -> forces R.B^T.B.(x1-x0)
// 5: stiffnes mat K
// 6: K.u=forces ->u
// 7: x1, y1 += w * u

#include <memory>
#include <tuple>
#include <vector>

#include "MeshFlattening.h"


using spMat = Eigen::SparseMatrix<double>;

namespace lscmrelax
{

class NullSpaceProjector: public Eigen::IdentityPreconditioner
{
  public:
    Eigen::MatrixXd null_space_1;
    Eigen::MatrixXd null_space_2;

    template<typename Rhs>
    inline Rhs solve(Rhs& b) const {
        return b - this->null_space_1 * (this->null_space_2 * b);
    }

    void setNullSpace(Eigen::MatrixXd null_space) {
        // normalize + orthogonalize the nullspace
        this->null_space_1 = null_space * ((null_space.transpose() * null_space).inverse());
        this->null_space_2 = null_space.transpose();
    }
};

using Vector3 = Eigen::Vector3d;
using Vector2 = Eigen::Vector2d;

class LscmRelax{
private:
    ColMat<double, 3> q_l_g;  // the position of the 3d triangles at there locale coord sys
    ColMat<double, 3> q_l_m;  // the mapped position in local coord sys

    void set_q_l_g();
    void set_q_l_m();
    void set_fixed_pins();
    void set_position(Eigen::VectorXd);
    void set_shift(Eigen::VectorXd);

    std::vector<long> new_order;
    std::vector<long> old_order;

    Eigen::Matrix<double, 3, 3> C;
    Eigen::VectorXd sol;

    std::vector<long> get_fem_fixed_pins();
    Eigen::MatrixXd get_nullspace();

public:
    LscmRelax() = default;
    LscmRelax(
        RowMat<double, 3> vertices,
        RowMat<long, 3> triangles,
        std::vector<long> fixed_pins);

    std::vector<long> fixed_pins;
    RowMat<double, 3> vertices;
    RowMat<long, 3> triangles;
    RowMat<double, 2> flat_vertices;
    ColMat<double, 1> rhs;
    Eigen::MatrixXd MATRIX;

    double nue=0.9;
    double elasticity=1.;

    void lscm();
    void relax(double);
    void area_relax(double);
    void edge_relax(double);

    ColMat<double, 3> get_flat_vertices_3D();

    void rotate_by_min_bound_area();
    void transform(bool scale=false);

    double get_area();
    double get_flat_area();

};

}

#endif
// clang-format on
