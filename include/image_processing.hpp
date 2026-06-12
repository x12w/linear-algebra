#ifndef IMAGE_PROCESSING_HPP
#define IMAGE_PROCESSING_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

#include <linear_algebra.hpp>

#ifdef LINEAR_ALGEBRA_HAS_OPENCV
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#endif

namespace image_processing {

using Image = linear_algebra::Matrix<double>;

struct ImageFeatures {
  double min_value;
  double max_value;
  double mean;
  double variance;
  double edge_density;
};

inline bool opencv_available() {
#ifdef LINEAR_ALGEBRA_HAS_OPENCV
  return true;
#else
  return false;
#endif
}

inline std::size_t clamp_index(long long value, std::size_t upper_bound) {
  if (upper_bound == 0) {
    return 0;
  }
  if (value < 0) {
    return 0;
  }
  std::size_t converted = static_cast<std::size_t>(value);
  if (converted >= upper_bound) {
    return upper_bound - 1;
  }
  return converted;
}

inline Image convolve2d(const Image &image, const Image &kernel) {
  if (kernel.row() == 0 || kernel.col() == 0) {
    throw(std::invalid_argument("error: convolution kernel cannot be empty"));
  }
  if (kernel.row() % 2 == 0 || kernel.col() % 2 == 0) {
    throw(std::invalid_argument(
        "error: convolution kernel must have odd row and column sizes"));
  }

  Image result(image.row(), image.col(), 0.0);
  long long row_radius = static_cast<long long>(kernel.row() / 2);
  long long col_radius = static_cast<long long>(kernel.col() / 2);

  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      double sum = 0;
      for (std::size_t kr = 0; kr < kernel.row(); kr++) {
        for (std::size_t kc = 0; kc < kernel.col(); kc++) {
          long long source_r =
              static_cast<long long>(r) + static_cast<long long>(kr) -
              row_radius;
          long long source_c =
              static_cast<long long>(c) + static_cast<long long>(kc) -
              col_radius;
          std::size_t rr = clamp_index(source_r, image.row());
          std::size_t cc = clamp_index(source_c, image.col());
          sum += image[rr][cc] * kernel[kr][kc];
        }
      }
      result[r][c] = sum;
    }
  }
  return result;
}

inline Image gaussian_kernel_3x3() {
  Image kernel(3, 3, 0.0);
  kernel[0][0] = 1.0 / 16.0;
  kernel[0][1] = 2.0 / 16.0;
  kernel[0][2] = 1.0 / 16.0;
  kernel[1][0] = 2.0 / 16.0;
  kernel[1][1] = 4.0 / 16.0;
  kernel[1][2] = 2.0 / 16.0;
  kernel[2][0] = 1.0 / 16.0;
  kernel[2][1] = 2.0 / 16.0;
  kernel[2][2] = 1.0 / 16.0;
  return kernel;
}

inline Image sobel_x_kernel() {
  Image kernel(3, 3, 0.0);
  kernel[0][0] = -1;
  kernel[0][1] = 0;
  kernel[0][2] = 1;
  kernel[1][0] = -2;
  kernel[1][1] = 0;
  kernel[1][2] = 2;
  kernel[2][0] = -1;
  kernel[2][1] = 0;
  kernel[2][2] = 1;
  return kernel;
}

inline Image sobel_y_kernel() {
  Image kernel(3, 3, 0.0);
  kernel[0][0] = -1;
  kernel[0][1] = -2;
  kernel[0][2] = -1;
  kernel[1][0] = 0;
  kernel[1][1] = 0;
  kernel[1][2] = 0;
  kernel[2][0] = 1;
  kernel[2][1] = 2;
  kernel[2][2] = 1;
  return kernel;
}

inline Image gradient_magnitude(const Image &gx, const Image &gy) {
  if (gx.row() != gy.row() || gx.col() != gy.col()) {
    throw(std::invalid_argument(
        "error: gradient matrices must have the same size"));
  }
  Image result(gx.row(), gx.col(), 0.0);
  for (std::size_t r = 0; r < gx.row(); r++) {
    for (std::size_t c = 0; c < gx.col(); c++) {
      result[r][c] = std::sqrt(gx[r][c] * gx[r][c] + gy[r][c] * gy[r][c]);
    }
  }
  return result;
}

inline Image sobel_edges(const Image &image) {
  Image smoothed = convolve2d(image, gaussian_kernel_3x3());
  Image gx = convolve2d(smoothed, sobel_x_kernel());
  Image gy = convolve2d(smoothed, sobel_y_kernel());
  return gradient_magnitude(gx, gy);
}

inline Image threshold(const Image &image, double threshold_value,
                       double low_value = 0.0, double high_value = 255.0) {
  Image result(image.row(), image.col(), low_value);
  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      result[r][c] = image[r][c] >= threshold_value ? high_value : low_value;
    }
  }
  return result;
}

inline Image normalize_to_byte_range(const Image &image) {
  if (image.row() == 0 || image.col() == 0) {
    return image;
  }
  double min_value = std::numeric_limits<double>::infinity();
  double max_value = -std::numeric_limits<double>::infinity();
  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      min_value = std::min(min_value, image[r][c]);
      max_value = std::max(max_value, image[r][c]);
    }
  }
  Image result(image.row(), image.col(), 0.0);
  double range = max_value - min_value;
  if (range <= 0) {
    return result;
  }
  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      result[r][c] = (image[r][c] - min_value) * 255.0 / range;
    }
  }
  return result;
}

inline ImageFeatures analyze_features(const Image &image,
                                      double edge_threshold = 128.0) {
  if (image.row() == 0 || image.col() == 0) {
    throw(std::invalid_argument("error: cannot analyze an empty image"));
  }

  ImageFeatures features{};
  features.min_value = std::numeric_limits<double>::infinity();
  features.max_value = -std::numeric_limits<double>::infinity();
  double sum = 0;
  double square_sum = 0;
  std::size_t edge_count = 0;
  std::size_t count = image.row() * image.col();

  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      double value = image[r][c];
      features.min_value = std::min(features.min_value, value);
      features.max_value = std::max(features.max_value, value);
      sum += value;
      square_sum += value * value;
      if (value >= edge_threshold) {
        edge_count++;
      }
    }
  }

  features.mean = sum / static_cast<double>(count);
  features.variance =
      square_sum / static_cast<double>(count) - features.mean * features.mean;
  features.edge_density = static_cast<double>(edge_count) /
                          static_cast<double>(count);
  return features;
}

inline void save_pgm(const Image &image, const std::string &path) {
  std::ofstream out(path);
  if (!out) {
    throw(std::ios_base::failure("error: cannot open PGM output file"));
  }
  out << "P2\n";
  out << image.col() << " " << image.row() << "\n";
  out << "255\n";
  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      int value = static_cast<int>(std::round(image[r][c]));
      value = std::max(0, std::min(255, value));
      out << value << (c + 1 == image.col() ? '\n' : ' ');
    }
  }
}

inline Image load_pgm(const std::string &path) {
  std::ifstream in(path);
  if (!in) {
    throw(std::ios_base::failure("error: cannot open PGM input file"));
  }
  std::string magic;
  in >> magic;
  if (magic != "P2") {
    throw(std::invalid_argument("error: only ASCII P2 PGM files are supported"));
  }
  std::size_t cols = 0;
  std::size_t rows = 0;
  int max_value = 0;
  in >> cols >> rows >> max_value;
  if (cols == 0 || rows == 0 || max_value <= 0) {
    throw(std::invalid_argument("error: invalid PGM header"));
  }
  Image image(rows, cols, 0.0);
  for (std::size_t r = 0; r < rows; r++) {
    for (std::size_t c = 0; c < cols; c++) {
      double value = 0;
      in >> value;
      image[r][c] = value * 255.0 / static_cast<double>(max_value);
    }
  }
  return image;
}

inline Image load_grayscale_image(const std::string &path) {
#ifdef LINEAR_ALGEBRA_HAS_OPENCV
  cv::Mat mat = cv::imread(path, cv::IMREAD_GRAYSCALE);
  if (mat.empty()) {
    throw(std::ios_base::failure("error: cannot open image file with OpenCV"));
  }
  Image image(static_cast<std::size_t>(mat.rows),
              static_cast<std::size_t>(mat.cols), 0.0);
  for (int r = 0; r < mat.rows; r++) {
    for (int c = 0; c < mat.cols; c++) {
      image[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] =
          static_cast<double>(mat.at<unsigned char>(r, c));
    }
  }
  return image;
#else
  return load_pgm(path);
#endif
}

inline void save_grayscale_image(const Image &image, const std::string &path) {
#ifdef LINEAR_ALGEBRA_HAS_OPENCV
  cv::Mat mat(static_cast<int>(image.row()), static_cast<int>(image.col()),
              CV_8UC1);
  for (std::size_t r = 0; r < image.row(); r++) {
    for (std::size_t c = 0; c < image.col(); c++) {
      int value = static_cast<int>(std::round(image[r][c]));
      value = std::max(0, std::min(255, value));
      mat.at<unsigned char>(static_cast<int>(r), static_cast<int>(c)) =
          static_cast<unsigned char>(value);
    }
  }
  if (!cv::imwrite(path, mat)) {
    throw(std::ios_base::failure("error: cannot write image file with OpenCV"));
  }
#else
  save_pgm(image, path);
#endif
}

} // namespace image_processing

#endif
