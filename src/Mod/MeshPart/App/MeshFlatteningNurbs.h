#ifndef NURBS_H
#define NURBS_H

#include <tuple>

#include <Eigen/IterativeLinearSolvers>


namespace nurbs{

using trip = Eigen::Triplet<double>;
using spMat = Eigen::SparseMatrix<double>;

struct NurbsBase2D
{
    //
    NurbsBase2D() = default;
    NurbsBase2D(Eigen::VectorXd u_knots, Eigen::VectorXd v_knots,
              Eigen::VectorXd weights,
              int degree_u=3, int degree_v=3);
    int degree_u;
    int degree_v;
    Eigen::VectorXd u_knots;
    Eigen::VectorXd v_knots;
    Eigen::VectorXd weights;

    std::vector<std::function<double(double)>> u_functions;
    std::vector<std::function<double(double)>> v_functions;

    std::vector<std::function<double(double)>> Du_functions;
    std::vector<std::function<double(double)>> Dv_functions;

    std::vector<std::function<double(double)>> DDu_functions;
    std::vector<std::function<double(double)>> DDv_functions;

    void computeFirstDerivatives();
    void computeSecondDerivatives();

    Eigen::VectorXd getInfluenceVector(Eigen::Vector2d u);
    spMat getInfluenceMatrix(Eigen::Matrix<double, Eigen::Dynamic, 2> U);

    Eigen::VectorXd getDuVector(Eigen::Vector2d u);
    spMat getDuMatrix(Eigen::Matrix<double, Eigen::Dynamic, 2> U);

    Eigen::VectorXd getDvVector(Eigen::Vector2d u);
    spMat getDvMatrix(Eigen::Matrix<double, Eigen::Dynamic, 2> U);

    Eigen::Matrix<double, Eigen::Dynamic, 2> getUVMesh(int num_u_points, int num_v_points);

    std::tuple<NurbsBase2D, Eigen::MatrixXd> interpolateUBS(
        Eigen::Matrix<double, Eigen::Dynamic, 3> poles,
        int degree_u,
        int degree_v,
        int num_u_poles,
        int num_v_poles,
        int num_u_points,
        int num_v_points);
};

struct NurbsBase1D
{
    NurbsBase1D() = default;
    NurbsBase1D(Eigen::VectorXd u_knots, Eigen::VectorXd weights, int degree_u=3);
    int degree_u;
    Eigen::VectorXd u_knots;
    Eigen::VectorXd weights;
    std::vector<std::function<double(double)>> u_functions;
    std::vector<std::function<double(double)>> Du_functions;
    std::vector<std::function<double(double)>> DDu_functions;

    void computeFirstDerivatives();
    void computeSecondDerivatives();

    Eigen::VectorXd getInfluenceVector(double u);
    spMat getInfluenceMatrix(Eigen::VectorXd u);

    Eigen::VectorXd getDuVector(double u);
    spMat getDuMatrix(Eigen::VectorXd u);

    static Eigen::VectorXd getKnotSequence(double u_min, double u_max, int deg, int num_poles);
    static Eigen::VectorXd getWeightList(Eigen::VectorXd knots, int u_deg);

    Eigen::VectorXd getUMesh(int num_u_points);

    std::tuple<NurbsBase1D, Eigen::Matrix<double, Eigen::Dynamic, 3>> interpolateUBS(
        Eigen::Matrix<double, Eigen::Dynamic, 3> poles,
        int degree,
        int num_u_poles,
        int num_u_points);

};

}

#endif
// clang-format on
