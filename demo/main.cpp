#include <basic_algebra.hpp>
#include <iomanip>
#include <iostream>
#include <linear_algebra.hpp>
#include <limits>
#include <stdexcept>
#include <string>

using basic_algebra::BigInteger;
using basic_algebra::ComplexNumber;
using basic_algebra::HighPrecisionNumber;
using basic_algebra::RationalNumber;
using linear_algebra::Matrix;
using linear_algebra::MatrixBase;
using linear_algebra::Vector;

namespace {

void print_section(const std::string &title) {
  std::cout << "\n========== " << title << " ==========\n";
}

void show_project_goal() {
  print_section("Project Goal");
  std::cout << "C++17 linear algebra library for vectors, matrices, common "
               "matrix algorithms,\n"
            << "norms, condition numbers, and custom numeric types.\n";
  std::cout << "Current milestone: task 1 core operations and task 2 norm "
               "features are implemented.\n";
}

void show_basic_number_types() {
  print_section("Basic Algebra Types");

  RationalNumber<long long> r1(2, 3);
  RationalNumber<long long> r2(5, 7);
  std::cout << "Rational: " << r1 << " + " << r2 << " = " << r1 + r2
            << "\n";

  ComplexNumber<double> z1(3.0, 4.0);
  ComplexNumber<double> z2(1.5, 2.0);
  std::cout << "Complex: (" << z1 << ") * (" << z2 << ") = " << z1 * z2
            << "\n";

  BigInteger<> big_a("123456789012345678901234567890");
  BigInteger<> big_b("98765432109876543210");
  std::cout << "BigInteger add: " << big_a + big_b << "\n";
  std::cout << "BigInteger multiply: " << big_b * BigInteger<>(1000000)
            << "\n";

  HighPrecisionNumber<> hp_a = 1.25;
  HighPrecisionNumber<> hp_b("2.5");
  std::cout << "HighPrecisionNumber: " << hp_a << " + " << hp_b
            << " = " << hp_a + hp_b << "\n";
  std::cout << "HighPrecisionNumber: " << hp_b << " / " << hp_a
            << " = " << hp_b / hp_a << "\n";
}

void show_vector_features() {
  print_section("Vector Operations And Norms");

  Vector<double> a(3);
  a[0] = 3.0;
  a[1] = 4.0;
  a[2] = -12.0;

  Vector<double> b(3);
  b[0] = 2.0;
  b[1] = -1.0;
  b[2] = 5.0;

  std::cout << "a = " << a << "\n";
  std::cout << "b = " << b << "\n";
  std::cout << "a + b = " << a + b << "\n";
  std::cout << "a dot b = " << a * b << "\n";
  std::cout << "a cross b = " << a.cross_product(b) << "\n";
  std::cout << "L1 norm(a) = " << a.norm(1) << "\n";
  std::cout << "L2 norm(a) = " << a.norm(2) << "\n";
  std::cout << "Infinity norm(a) = " << a.infinity_norm() << "\n";
}

void show_matrix_features() {
  print_section("Matrix Operations");

  Matrix<double> a(3, 3);
  a[0][0] = 4.0;
  a[0][1] = 2.0;
  a[0][2] = 0.0;
  a[1][0] = 2.0;
  a[1][1] = 5.0;
  a[1][2] = 1.0;
  a[2][0] = 0.0;
  a[2][1] = 1.0;
  a[2][2] = 3.0;

  Matrix<double> b(3, 3);
  b[0][0] = 1.0;
  b[0][1] = 0.0;
  b[0][2] = 2.0;
  b[1][0] = 0.0;
  b[1][1] = 1.0;
  b[1][2] = 0.0;
  b[2][0] = 2.0;
  b[2][1] = 0.0;
  b[2][2] = 1.0;

  std::cout << "A:\n" << a;
  std::cout << "B:\n" << b;
  std::cout << "A + B:\n" << a + b;
  std::cout << "A * B:\n" << a * b;
  std::cout << "Transpose(A):\n" << a.get_transpose();
  std::cout << "det(A) = " << a.determinant() << "\n";
  std::cout << "rank(A) = " << a.rank() << "\n";
  std::cout << "inverse(A):\n" << a.inverse();
  std::cout << "reduced row echelon form(A):\n"
            << a.reduced_row_echelon_form();

  MatrixBase<double> *base_view = &a;
  std::cout << "MatrixBase polymorphic view: rows = " << base_view->row()
            << ", cols = " << base_view->col() << "\n";
}

void show_task_two_features() {
  print_section("Task 2 Norms And Condition Number");

  Matrix<double> a(3, 3);
  a[0][0] = 4.0;
  a[0][1] = 2.0;
  a[0][2] = 0.0;
  a[1][0] = 2.0;
  a[1][1] = 5.0;
  a[1][2] = 1.0;
  a[2][0] = 0.0;
  a[2][1] = 1.0;
  a[2][2] = 3.0;

  std::cout << "Frobenius norm(A) = " << a.frobenius_norm() << "\n";
  std::cout << "Row-sum norm(A) = " << a.row_sum_norm() << "\n";
  std::cout << "Column-sum norm(A) = " << a.col_sum_norm() << "\n";
  std::cout << "Spectral norm(A) ~= " << a.spectral_norm() << "\n";
  std::cout << "Condition number(A, row-sum norm) = "
            << a.condition_number() << "\n";

  Matrix<double> singular(2, 2);
  singular[0][0] = 1.0;
  singular[0][1] = 2.0;
  singular[1][0] = 2.0;
  singular[1][1] = 4.0;

  long double cond = singular.condition_number();
  std::cout << "Condition number(singular matrix) = ";
  if (cond == std::numeric_limits<long double>::infinity()) {
    std::cout << "infinity\n";
  } else {
    std::cout << cond << "\n";
  }
}

void show_exception_handling() {
  print_section("Exception Handling Demo");

  try {
    Vector<double> a(2);
    Vector<double> b(3);
    std::cout << a * b << "\n";
  } catch (const std::exception &ex) {
    std::cout << "Caught dimension error: " << ex.what() << "\n";
  }

  try {
    Matrix<double> non_square(2, 3);
    std::cout << non_square.determinant() << "\n";
  } catch (const std::exception &ex) {
    std::cout << "Caught determinant error: " << ex.what() << "\n";
  }
}

} // namespace

int main() {
  std::cout << std::fixed << std::setprecision(6);

  show_project_goal();
  show_basic_number_types();
  show_vector_features();
  show_matrix_features();
  show_task_two_features();
  show_exception_handling();

  print_section("Midterm Summary");
  std::cout << "The project now has reusable template numeric types, vector and "
               "matrix containers,\n"
            << "core linear algebra algorithms, task 2 norm calculations, and "
               "basic exception handling.\n";

  return 0;
}
