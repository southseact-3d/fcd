#include <Eigen/Dense>

int qp_eq(
    Eigen::MatrixXd& H,
    Eigen::VectorXd& g,
    Eigen::MatrixXd& A,
    Eigen::VectorXd& c,
    Eigen::VectorXd& x,
    Eigen::MatrixXd& Y,
    Eigen::MatrixXd& Z
);
