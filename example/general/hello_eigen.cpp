// hello_eigen.cpp
#include <iostream>
#include <Eigen/Dense>

int main() {
    // Define a 2x2 matrix and a 2x1 vector
    Eigen::Matrix2d A;
    Eigen::Vector2d b;

    // Initialize them
    A << 1, 2,
         3, 4;
    b << 5,
         6;

    // Solve the linear system A * x = b
    Eigen::Vector2d x = A.colPivHouseholderQr().solve(b);

    std::cout << "Solution x:\n" << x << '\n';
    return 0;
}
