#include <basic_algebra.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <linear_algebra.hpp>

using namespace basic_algebra;
using namespace linear_algebra;

int main() {
  std::cout << std::fixed << std::setprecision(6);

  Vector<double> v(3);
  v[0] = 3;
  v[1] = 4;
  v[2] = -12;
  std::cout << "Vector v: " << v << '\n';
  std::cout << "L1 norm: " << v.norm(1) << '\n';
  std::cout << "L2 norm: " << v.norm(2) << '\n';
  std::cout << "Infinity norm: " << v.infinity_norm() << "\n\n";

  Matrix<double> a(2, 2);
  a[0][0] = 1;
  a[0][1] = 2;
  a[1][0] = 3;
  a[1][1] = 4;
  std::cout << "Matrix A:\n" << a;
  std::cout << "Frobenius norm: " << a.frobenius_norm() << '\n';
  std::cout << "Row-sum norm: " << a.row_sum_norm() << '\n';
  std::cout << "Column-sum norm: " << a.col_sum_norm() << '\n';
  std::cout << "Spectral norm: " << a.spectral_norm() << '\n';
  std::cout << "Condition number(row-sum): " << a.condition_number() << "\n\n";
  std::cout << "Determinant: " << a.determinant() << '\n';
  std::cout << "Rank: " << a.rank() << '\n';
  std::cout << "Inverse:\n" << a.inverse() << '\n';

  Matrix<double> system(3, 3);
  system[0][0] = 3;
  system[0][1] = 2;
  system[0][2] = -1;
  system[1][0] = 2;
  system[1][1] = -2;
  system[1][2] = 4;
  system[2][0] = -1;
  system[2][1] = 0.5;
  system[2][2] = -1;
  Vector<double> rhs(3);
  rhs[0] = 1;
  rhs[1] = -2;
  rhs[2] = 0;
  std::cout << "Solve square linear system Ax=b: x = " << system.solve(rhs)
            << "\n\n";

  Matrix<double> least_square_matrix(4, 2);
  least_square_matrix[0][0] = 1;
  least_square_matrix[0][1] = 1;
  least_square_matrix[1][0] = 1;
  least_square_matrix[1][1] = 2;
  least_square_matrix[2][0] = 1;
  least_square_matrix[2][1] = 3;
  least_square_matrix[3][0] = 1;
  least_square_matrix[3][1] = 4;
  Vector<double> observations(4);
  observations[0] = 6;
  observations[1] = 5;
  observations[2] = 7;
  observations[3] = 10;
  std::cout << "Least squares line y = c0 + c1*x: coefficients = "
            << least_square_matrix.least_squares(observations) << "\n\n";
  std::cout << "QR least squares coefficients = "
            << least_square_matrix.least_squares_qr(observations) << "\n\n";

  Matrix<double> eigen_matrix(2, 2);
  eigen_matrix[0][0] = 2;
  eigen_matrix[0][1] = 1;
  eigen_matrix[1][0] = 1;
  eigen_matrix[1][1] = 2;
  auto eigen = eigen_matrix.dominant_eigenpair();
  std::cout << "Dominant eigenvalue: " << eigen.first << '\n';
  std::cout << "Dominant eigenvector: " << eigen.second << "\n\n";
  std::cout << "QR eigenvalues: " << eigen_matrix.eigenvalues_qr() << "\n\n";

  const std::size_t performance_size = 96;
  Matrix<double> left(performance_size, performance_size);
  Matrix<double> right(performance_size, performance_size);
  for (std::size_t r = 0; r < performance_size; r++) {
    for (std::size_t c = 0; c < performance_size; c++) {
      left[r][c] = static_cast<double>((r + c) % 11 + 1);
      right[r][c] = static_cast<double>((r == c) ? 2 : 1);
    }
  }
  auto normal_start = std::chrono::high_resolution_clock::now();
  Matrix<double> normal_product = left * right;
  auto normal_end = std::chrono::high_resolution_clock::now();
  auto block_start = std::chrono::high_resolution_clock::now();
  Matrix<double> block_product = left.block_multiply(right, 16);
  auto block_end = std::chrono::high_resolution_clock::now();
  auto parallel_block_start = std::chrono::high_resolution_clock::now();
  Matrix<double> parallel_block_product =
      left.parallel_block_multiply(right, 16, 4);
  auto parallel_block_end = std::chrono::high_resolution_clock::now();
  std::cout << "Block multiplication equals normal multiplication: "
            << (block_product == normal_product ? "true" : "false") << '\n';
  std::cout << "Parallel block multiplication equals normal multiplication: "
            << (parallel_block_product == normal_product ? "true" : "false")
            << '\n';
  std::cout << "Parallel Frobenius norm(left): "
            << left.parallel_frobenius_norm(2) << '\n';
  std::cout << "Normal multiply time(ns): "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   normal_end - normal_start)
                   .count()
            << '\n';
  std::cout << "Block multiply time(ns): "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   block_end - block_start)
                   .count()
            << '\n';
  std::cout << "Parallel block multiply time(ns): "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   parallel_block_end - parallel_block_start)
                   .count()
            << "\n\n";

  BigInteger<> big1("123456789012345678901234567890");
  BigInteger<> big2("9876543210");
  std::cout << "BigInteger sum: " << big1 + big2 << '\n';
  std::cout << "BigInteger product: " << big2 * BigInteger<>(100000) << '\n';

  HighPrecisionNumber<> hp1("1.25");
  HighPrecisionNumber<> hp2("2.5");
  std::cout << "HighPrecision sum: " << hp1 + hp2 << '\n';
  std::cout << "HighPrecision division: " << hp2 / hp1 << '\n';
  HighPrecisionNumber<>::set_default_division_precision(50);
  std::cout << "HighPrecision division precision: "
            << HighPrecisionNumber<>::default_division_precision() << '\n';
  std::cout << "HighPrecision 1/7: "
            << HighPrecisionNumber<>(1LL) / HighPrecisionNumber<>(7LL) << '\n';

  return 0;
}
