#include <basic_algebra.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <gpu_acceleration.hpp>
#include <iomanip>
#include <image_processing.hpp>
#include <iostream>
#include <linear_algebra.hpp>
#include <stdexcept>
#include <string>

using namespace basic_algebra;
using namespace gpu_acceleration;
using namespace image_processing;
using namespace linear_algebra;

namespace {

std::filesystem::path project_root = ".";
std::filesystem::path data_dir = "data";
std::filesystem::path output_dir = "output";

bool has_demo_data(const std::filesystem::path &path) {
  return std::filesystem::exists(path / "data" / "vector_a.txt");
}

std::filesystem::path locate_project_root(const char *executable_path) {
  std::filesystem::path current = std::filesystem::current_path();
  if (has_demo_data(current)) {
    return current;
  }
  if (has_demo_data(current.parent_path())) {
    return current.parent_path();
  }

  std::filesystem::path executable = std::filesystem::absolute(executable_path);
  std::filesystem::path executable_dir = executable.parent_path();
  if (has_demo_data(executable_dir)) {
    return executable_dir;
  }
  if (has_demo_data(executable_dir.parent_path())) {
    return executable_dir.parent_path();
  }

  throw(std::ios_base::failure(
      "error: cannot locate demo data directory; run from the project root or "
      "keep the data directory next to the project"));
}

std::string path_string(const std::filesystem::path &path) {
  return path.string();
}

Vector<double> load_vector(const std::string &path) {
  std::ifstream in(path);
  if (!in) {
    throw(std::ios_base::failure("error: cannot open vector input file"));
  }
  std::size_t size = 0;
  in >> size;
  Vector<double> result(size, 0.0);
  for (std::size_t i = 0; i < size; i++) {
    in >> result[i];
  }
  return result;
}

Matrix<double> make_benchmark_matrix(std::size_t rows, std::size_t cols,
                                     double offset) {
  Matrix<double> result(rows, cols, 0.0);
  for (std::size_t r = 0; r < rows; r++) {
    for (std::size_t c = 0; c < cols; c++) {
      double base = static_cast<double>(((r + 3) * (c + 5)) % 17);
      double wave = static_cast<double>((r + c) % 7) * 0.125;
      result[r][c] = base + wave + offset;
    }
  }
  return result;
}

Matrix<double> legacy_column_fetch_multiply(const Matrix<double> &lhs,
                                            const Matrix<double> &rhs) {
  if (lhs.col() != rhs.row()) {
    throw(std::runtime_error("error: legacy matrix multiplication size "
                             "mismatch"));
  }
  Matrix<double> result(lhs.row(), rhs.col(), 0.0);
  for (std::size_t r = 0; r < result.row(); r++) {
    for (std::size_t c = 0; c < result.col(); c++) {
      result[r][c] = lhs[r] * rhs.fetch_col(c);
    }
  }
  return result;
}

template <typename Func> auto time_matrix_call(Func func) {
  auto start = std::chrono::high_resolution_clock::now();
  Matrix<double> result = func();
  auto end = std::chrono::high_resolution_clock::now();
  long long nanos =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
          .count();
  return std::make_pair(result, nanos);
}

void print_timing(const std::string &name, long long nanos,
                  long long baseline) {
  std::cout << std::left << std::setw(28) << name << std::right
            << std::setw(14) << nanos << " ns";
  if (baseline > 0 && nanos > 0) {
    double speedup = static_cast<double>(baseline) / static_cast<double>(nanos);
    std::cout << "  speedup: " << std::fixed << std::setprecision(2)
              << speedup << "x";
  }
  std::cout << '\n';
}

void print_title(const std::string &title) {
  std::cout << "\n==== " << title << " ====\n";
}

template <typename Func> void run_demo(const std::string &title, Func func) {
  print_title(title);
  try {
    func();
  } catch (const std::exception &ex) {
    std::cout << "操作失败: " << ex.what() << '\n';
  }
}

void show_vector_demo() {
  Vector<double> a = load_vector(path_string(data_dir / "vector_a.txt"));
  Vector<double> b = load_vector(path_string(data_dir / "vector_b.txt"));
  std::cout << "a = " << a << '\n';
  std::cout << "b = " << b << '\n';
  std::cout << "a + b = " << a + b << '\n';
  std::cout << "a - b = " << a - b << '\n';
  std::cout << "a dot b = " << a * b << '\n';
  std::cout << "a cross b = " << a.cross_product(b) << '\n';
  std::cout << "||a||_2 = " << a.norm(2) << '\n';
  std::cout << "||a||_inf = " << a.infinity_norm() << '\n';
}

void show_matrix_demo() {
  Matrix<double> a =
      Matrix<double>::load_from_file(path_string(data_dir / "matrix_a.txt"));
  Matrix<double> b =
      Matrix<double>::load_from_file(path_string(data_dir / "matrix_b.txt"));
  std::cout << "A:\n" << a;
  std::cout << "B:\n" << b;
  std::cout << "A + B:\n" << a + b;
  std::cout << "A - B:\n" << a - b;
  std::cout << "A * B:\n" << a * b;
  std::cout << "transpose(A):\n" << a.get_transpose();
  std::cout << "det(A) = " << a.determinant() << '\n';
  std::cout << "inverse(A):\n" << a.inverse();
}

void show_solve_demo() {
  Matrix<double> a =
      Matrix<double>::load_from_file(path_string(data_dir / "linear_system_A.txt"));
  Vector<double> b = load_vector(path_string(data_dir / "linear_system_b.txt"));
  Vector<double> x = a.solve(b);
  std::cout << "A:\n" << a;
  std::cout << "b = " << b << '\n';
  std::cout << "solve(A, b) = " << x << '\n';
  std::cout << "A * x = " << a * x << '\n';
}

void show_least_squares_demo() {
  Matrix<double> design =
      Matrix<double>::load_from_file(path_string(data_dir / "least_squares_A.txt"));
  Vector<double> y = load_vector(path_string(data_dir / "least_squares_b.txt"));
  Vector<double> normal_result = design.least_squares(y);
  Vector<long double> qr_result = design.least_squares_qr(y);
  std::cout << "Design matrix:\n" << design;
  std::cout << "observations = " << y << '\n';
  std::cout << "least_squares(normal equation) = " << normal_result << '\n';
  std::cout << "least_squares_qr = " << qr_result << '\n';
}

void show_eigen_norm_demo() {
  Matrix<double> a =
      Matrix<double>::load_from_file(path_string(data_dir / "eigen_matrix.txt"));
  auto dominant = a.dominant_eigenpair();
  std::cout << "A:\n" << a;
  std::cout << "row sum norm = " << a.row_sum_norm() << '\n';
  std::cout << "column sum norm = " << a.col_sum_norm() << '\n';
  std::cout << "Frobenius norm = " << a.frobenius_norm() << '\n';
  std::cout << "parallel Frobenius norm = " << a.parallel_frobenius_norm()
            << '\n';
  std::cout << "condition number(row norm) = " << a.condition_number()
            << '\n';
  std::cout << "dominant eigenvalue = " << dominant.first << '\n';
  std::cout << "dominant eigenvector = " << dominant.second << '\n';
  std::cout << "QR eigenvalues = " << a.eigenvalues_qr() << '\n';
}

void show_performance_demo() {
  Matrix<double> a =
      Matrix<double>::load_from_file(path_string(data_dir / "perf_matrix_a.txt"));
  Matrix<double> b =
      Matrix<double>::load_from_file(path_string(data_dir / "perf_matrix_b.txt"));

  auto time_call = [](auto &&callable) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = callable();
    auto end = std::chrono::high_resolution_clock::now();
    long long nanos =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    return std::make_pair(result, nanos);
  };

  auto normal = time_call([&]() { return a * b; });
  auto blocked = time_call([&]() { return a.block_multiply(b, 16); });
  auto strassen = time_call([&]() { return a.strassen_multiply(b, 32); });
  auto parallel =
      time_call([&]() { return a.parallel_block_multiply(b, 16, 4); });

  std::cout << "matrix size: " << a.row() << " x " << a.col() << '\n';
  std::cout << "block equals normal: "
            << (blocked.first == normal.first ? "true" : "false") << '\n';
  std::cout << "Strassen equals normal: "
            << (strassen.first == normal.first ? "true" : "false") << '\n';
  std::cout << "parallel block equals normal: "
            << (parallel.first == normal.first ? "true" : "false") << '\n';
  std::cout << "normal multiply(ns): " << normal.second << '\n';
  std::cout << "block multiply(ns): " << blocked.second << '\n';
  std::cout << "Strassen multiply(ns): " << strassen.second << '\n';
  std::cout << "parallel block multiply(ns): " << parallel.second << '\n';

  try {
    auto gpu = time_call([&]() { return opencl_matrix_multiply(a, b); });
    std::cout << "OpenCL device: " << opencl_device_name() << '\n';
    std::cout << "OpenCL equals normal: "
              << (gpu.first == normal.first ? "true" : "false") << '\n';
    std::cout << "OpenCL multiply(ns): " << gpu.second << '\n';
  } catch (const std::exception &ex) {
    std::cout << "OpenCL skipped: " << ex.what() << '\n';
  }
}

void show_before_after_performance_demo() {
  const std::size_t size = 512;
  Matrix<double> a = make_benchmark_matrix(size, size, 0.25);
  Matrix<double> b = make_benchmark_matrix(size, size, 1.0);

  auto legacy = time_matrix_call([&]() {
    return legacy_column_fetch_multiply(a, b);
  });
  auto optimized = time_matrix_call([&]() { return a * b; });
  auto blocked = time_matrix_call([&]() { return a.block_multiply(b, 32); });
  auto strassen =
      time_matrix_call([&]() { return a.strassen_multiply(b, 128); });
  auto parallel =
      time_matrix_call([&]() { return a.parallel_block_multiply(b, 32, 4); });

  std::cout << "benchmark matrix size: " << size << " x " << size << '\n';
  std::cout << "baseline: 优化前列向量 fetch_col + dot product 实现\n";
  std::cout << "optimized operator* equals baseline: "
            << (optimized.first == legacy.first ? "true" : "false") << '\n';
  std::cout << "block equals baseline: "
            << (blocked.first == legacy.first ? "true" : "false") << '\n';
  std::cout << "Strassen equals baseline: "
            << (strassen.first == legacy.first ? "true" : "false") << '\n';
  std::cout << "parallel block equals baseline: "
            << (parallel.first == legacy.first ? "true" : "false") << '\n';

  std::cout << "\n用时对比:\n";
  print_timing("优化前普通乘法", legacy.second, legacy.second);
  print_timing("优化后 operator*", optimized.second, legacy.second);
  print_timing("优化后 block_multiply", blocked.second, legacy.second);
  print_timing("优化后 Strassen", strassen.second, legacy.second);
  print_timing("优化后 parallel_block", parallel.second, legacy.second);

  try {
    std::cout << "\nOpenCL warmup: 初始化 runtime/program 并预热同规模 kernel...\n";
    Matrix<double> warmup = opencl_matrix_multiply(a, b);
    std::cout << "OpenCL warmup equals baseline: "
              << (warmup == legacy.first ? "true" : "false") << '\n';
    auto gpu = time_matrix_call([&]() { return opencl_matrix_multiply(a, b); });
    std::cout << "OpenCL device: " << opencl_device_name() << '\n';
    std::cout << "OpenCL equals baseline: "
              << (gpu.first == legacy.first ? "true" : "false") << '\n';
    print_timing("优化后 OpenCL", gpu.second, legacy.second);
  } catch (const std::exception &ex) {
    std::cout << "OpenCL skipped: " << ex.what() << '\n';
  }

  std::cout << "\n说明: 这里对 OpenCL 做了一次同规模预热，表格中的 OpenCL "
               "用时不包含首次 runtime/program 初始化。\n";
}

void show_image_demo() {
  std::filesystem::create_directories(output_dir);
  Image image = load_pgm(path_string(data_dir / "sample_image.pgm"));
  Image blur = convolve2d(image, gaussian_kernel_3x3());
  Image edges = normalize_to_byte_range(sobel_edges(image));
  Image binary = threshold(edges, 90.0);
  ImageFeatures features = analyze_features(edges, 90.0);
  save_pgm(blur, path_string(output_dir / "menu_blur.pgm"));
  save_pgm(edges, path_string(output_dir / "menu_edges.pgm"));
  save_pgm(binary, path_string(output_dir / "menu_binary_edges.pgm"));

  std::cout << "input image: " << data_dir / "sample_image.pgm" << '\n';
  std::cout << "saved: " << output_dir / "menu_blur.pgm" << '\n';
  std::cout << "saved: " << output_dir / "menu_edges.pgm" << '\n';
  std::cout << "saved: " << output_dir / "menu_binary_edges.pgm" << '\n';
  std::cout << "edge min/max = " << features.min_value << " / "
            << features.max_value << '\n';
  std::cout << "edge mean = " << features.mean << '\n';
  std::cout << "edge variance = " << features.variance << '\n';
  std::cout << "edge density = " << features.edge_density << '\n';

  try {
    Image opencl_blur = opencl_convolve3x3(image, gaussian_kernel_3x3());
    std::cout << "OpenCL 3x3 blur equals CPU blur: "
              << (opencl_blur == blur ? "true" : "false") << '\n';
  } catch (const std::exception &ex) {
    std::cout << "OpenCL convolution skipped: " << ex.what() << '\n';
  }
}

void show_number_demo() {
  ComplexNumber<double> c1(3.0, 4.0);
  ComplexNumber<double> c2(1.5, -2.0);
  RationalNumber<> r1(7, 12);
  RationalNumber<> r2(5, 18);
  BigInteger<> big1("123456789012345678901234567890");
  BigInteger<> big2("98765432109876543210");
  HighPrecisionNumber<> hp1("3.141592653589793238462643383279");
  HighPrecisionNumber<> hp2("2.718281828459045235360287471352");

  std::cout << "c1 = " << c1 << ", c2 = " << c2 << '\n';
  std::cout << "c1 + c2 = " << c1 + c2 << '\n';
  std::cout << "c1 * c2 = " << c1 * c2 << '\n';
  std::cout << "r1 = " << r1 << ", r2 = " << r2 << '\n';
  std::cout << "r1 + r2 = " << r1 + r2 << '\n';
  std::cout << "r1 * r2 = " << r1 * r2 << '\n';
  std::cout << "big1 + big2 = " << big1 + big2 << '\n';
  std::cout << "big1 * 12 = " << big1 * BigInteger<>(12) << '\n';
  std::cout << "hp1 + hp2 = " << hp1 + hp2 << '\n';
  std::cout << "hp1 * hp2 = " << hp1 * hp2 << '\n';
}

void print_menu() {
  std::cout << "\n====== 线性代数计算库交互演示 ======\n";
  std::cout << "1. 向量运算\n";
  std::cout << "2. 矩阵基础运算\n";
  std::cout << "3. 线性方程组求解\n";
  std::cout << "4. 最小二乘拟合\n";
  std::cout << "5. 范数、条件数和特征值\n";
  std::cout << "6. 矩阵乘法性能对比\n";
  std::cout << "7. 图像卷积和边缘特征分析\n";
  std::cout << "8. 基础数值类型演示\n";
  std::cout << "9. 优化前后计算用时对比\n";
  std::cout << "0. 退出\n";
  std::cout << "请选择操作编号: ";
}

} // namespace

int main(int argc, char **argv) {
  std::cout << std::fixed << std::setprecision(6);
  try {
    project_root = locate_project_root(argc > 0 ? argv[0] : ".");
    data_dir = project_root / "data";
    output_dir = project_root / "output";
  } catch (const std::exception &ex) {
    std::cout << ex.what() << '\n';
    return 1;
  }

  std::cout << "数据目录: " << data_dir << '\n';
  std::cout << "输出目录: " << output_dir << '\n';
  std::cout << "OpenCL编译状态: "
            << (opencl_compiled()
                    ? "已启用，运行时仍需可用的 OpenCL 平台"
                    : "未启用，请使用 nix-shell 配置 build-nix")
            << '\n';

  while (true) {
    print_menu();
    int choice = -1;
    if (!(std::cin >> choice)) {
      std::cout << "\n输入结束，退出演示。\n";
      break;
    }

    switch (choice) {
    case 1:
      run_demo("向量运算", show_vector_demo);
      break;
    case 2:
      run_demo("矩阵基础运算", show_matrix_demo);
      break;
    case 3:
      run_demo("线性方程组求解", show_solve_demo);
      break;
    case 4:
      run_demo("最小二乘拟合", show_least_squares_demo);
      break;
    case 5:
      run_demo("范数、条件数和特征值", show_eigen_norm_demo);
      break;
    case 6:
      run_demo("矩阵乘法性能对比", show_performance_demo);
      break;
    case 7:
      run_demo("图像卷积和边缘特征分析", show_image_demo);
      break;
    case 8:
      run_demo("基础数值类型演示", show_number_demo);
      break;
    case 9:
      run_demo("优化前后计算用时对比", show_before_after_performance_demo);
      break;
    case 0:
      std::cout << "演示结束。\n";
      return 0;
    default:
      std::cout << "无效编号，请重新选择。\n";
      break;
    }
  }
  return 0;
}
