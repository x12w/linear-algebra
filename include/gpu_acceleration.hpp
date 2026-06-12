#ifndef GPU_ACCELERATION_HPP
#define GPU_ACCELERATION_HPP

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <image_processing.hpp>
#include <linear_algebra.hpp>

#ifdef LINEAR_ALGEBRA_HAS_OPENCL
#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 120
#endif
#include <CL/cl.h>
#endif

namespace gpu_acceleration {

using MatrixD = linear_algebra::Matrix<double>;

inline bool opencl_compiled() {
#ifdef LINEAR_ALGEBRA_HAS_OPENCL
  return true;
#else
  return false;
#endif
}

#ifdef LINEAR_ALGEBRA_HAS_OPENCL
namespace detail {

inline void check(cl_int status, const std::string &message) {
  if (status != CL_SUCCESS) {
    std::ostringstream oss;
    oss << message << " (OpenCL error " << status << ")";
    throw(std::runtime_error(oss.str()));
  }
}

class OpenCLRuntime {
private:
  cl_platform_id platform = nullptr;
  cl_device_id device = nullptr;
  cl_context context = nullptr;
  cl_command_queue queue = nullptr;

  static bool device_supports_fp64(cl_device_id candidate) {
    std::size_t size = 0;
    clGetDeviceInfo(candidate, CL_DEVICE_EXTENSIONS, 0, nullptr, &size);
    std::string extensions(size, '\0');
    clGetDeviceInfo(candidate, CL_DEVICE_EXTENSIONS, size, extensions.data(),
                    nullptr);
    return extensions.find("cl_khr_fp64") != std::string::npos ||
           extensions.find("cl_amd_fp64") != std::string::npos;
  }

public:
  OpenCLRuntime() {
    cl_uint platform_count = 0;
    check(clGetPlatformIDs(0, nullptr, &platform_count),
          "failed to query OpenCL platforms");
    if (platform_count == 0) {
      throw(std::runtime_error("error: no OpenCL platform found"));
    }

    std::vector<cl_platform_id> platforms(platform_count);
    check(clGetPlatformIDs(platform_count, platforms.data(), nullptr),
          "failed to get OpenCL platforms");

    for (cl_platform_id candidate_platform : platforms) {
      for (cl_device_type type : {CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_CPU}) {
        cl_uint device_count = 0;
        cl_int status =
            clGetDeviceIDs(candidate_platform, type, 0, nullptr, &device_count);
        if (status != CL_SUCCESS || device_count == 0) {
          continue;
        }
        std::vector<cl_device_id> devices(device_count);
        status = clGetDeviceIDs(candidate_platform, type, device_count,
                                devices.data(), nullptr);
        if (status != CL_SUCCESS) {
          continue;
        }
        for (cl_device_id candidate_device : devices) {
          if (device_supports_fp64(candidate_device)) {
            platform = candidate_platform;
            device = candidate_device;
            break;
          }
        }
        if (device != nullptr) {
          break;
        }
      }
      if (device != nullptr) {
        break;
      }
    }

    if (device == nullptr) {
      throw(std::runtime_error(
          "error: no OpenCL device with double precision support found"));
    }

    cl_int status = CL_SUCCESS;
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);
    check(status, "failed to create OpenCL context");
    queue = clCreateCommandQueue(context, device, 0, &status);
    check(status, "failed to create OpenCL command queue");
  }

  ~OpenCLRuntime() {
    if (queue != nullptr) {
      clReleaseCommandQueue(queue);
    }
    if (context != nullptr) {
      clReleaseContext(context);
    }
  }

  cl_context get_context() const { return context; }
  cl_command_queue get_queue() const { return queue; }
  cl_device_id get_device() const { return device; }

  std::string device_name() const {
    std::size_t size = 0;
    clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &size);
    std::string name(size, '\0');
    clGetDeviceInfo(device, CL_DEVICE_NAME, size, name.data(), nullptr);
    if (!name.empty() && name.back() == '\0') {
      name.pop_back();
    }
    return name;
  }
};

inline cl_program build_program(const OpenCLRuntime &runtime,
                                const char *source) {
  cl_int status = CL_SUCCESS;
  std::size_t source_size = std::char_traits<char>::length(source);
  cl_program program = clCreateProgramWithSource(
      runtime.get_context(), 1, &source, &source_size, &status);
  check(status, "failed to create OpenCL program");

  cl_device_id device = runtime.get_device();
  status = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
  if (status != CL_SUCCESS) {
    std::size_t log_size = 0;
    clGetProgramBuildInfo(program, runtime.get_device(), CL_PROGRAM_BUILD_LOG,
                          0, nullptr, &log_size);
    std::string log(log_size, '\0');
    clGetProgramBuildInfo(program, runtime.get_device(), CL_PROGRAM_BUILD_LOG,
                          log_size, log.data(), nullptr);
    clReleaseProgram(program);
    throw(std::runtime_error("OpenCL build failed: " + log));
  }
  return program;
}

class ProgramHolder {
private:
  cl_program program = nullptr;

public:
  ProgramHolder(const OpenCLRuntime &runtime, const char *source)
      : program(build_program(runtime, source)) {}
  ProgramHolder(const ProgramHolder &) = delete;
  ProgramHolder &operator=(const ProgramHolder &) = delete;
  ~ProgramHolder() {
    if (program != nullptr) {
      clReleaseProgram(program);
    }
  }

  cl_program get() const { return program; }
};

class UniqueKernel {
private:
  cl_kernel kernel = nullptr;

public:
  explicit UniqueKernel(cl_kernel value) : kernel(value) {}
  UniqueKernel(const UniqueKernel &) = delete;
  UniqueKernel &operator=(const UniqueKernel &) = delete;
  UniqueKernel(UniqueKernel &&other) noexcept : kernel(other.kernel) {
    other.kernel = nullptr;
  }
  UniqueKernel &operator=(UniqueKernel &&other) noexcept {
    if (this != &other) {
      if (kernel != nullptr) {
        clReleaseKernel(kernel);
      }
      kernel = other.kernel;
      other.kernel = nullptr;
    }
    return *this;
  }
  ~UniqueKernel() {
    if (kernel != nullptr) {
      clReleaseKernel(kernel);
    }
  }

  cl_kernel get() const { return kernel; }
};

class UniqueMem {
private:
  cl_mem memory = nullptr;

public:
  explicit UniqueMem(cl_mem value) : memory(value) {}
  UniqueMem(const UniqueMem &) = delete;
  UniqueMem &operator=(const UniqueMem &) = delete;
  UniqueMem(UniqueMem &&other) noexcept : memory(other.memory) {
    other.memory = nullptr;
  }
  UniqueMem &operator=(UniqueMem &&other) noexcept {
    if (this != &other) {
      if (memory != nullptr) {
        clReleaseMemObject(memory);
      }
      memory = other.memory;
      other.memory = nullptr;
    }
    return *this;
  }
  ~UniqueMem() {
    if (memory != nullptr) {
      clReleaseMemObject(memory);
    }
  }

  cl_mem get() const { return memory; }
};

inline UniqueKernel make_kernel(cl_program program, const char *name) {
  cl_int status = CL_SUCCESS;
  cl_kernel kernel = clCreateKernel(program, name, &status);
  check(status, std::string("failed to create OpenCL ") + name + " kernel");
  return UniqueKernel(kernel);
}

inline UniqueMem make_buffer(cl_context context, cl_mem_flags flags,
                             std::size_t size, void *host_ptr,
                             const std::string &message) {
  cl_int status = CL_SUCCESS;
  cl_mem buffer = clCreateBuffer(context, flags, size, host_ptr, &status);
  check(status, message);
  return UniqueMem(buffer);
}

inline const char *matrix_multiply_source() {
  return R"CLC(
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void matmul(__global const double* a,
                     __global const double* b,
                     __global double* c,
                     const int rows_a,
                     const int cols_a,
                     const int cols_b) {
  int row = get_global_id(0);
  int col = get_global_id(1);
  if (row >= rows_a || col >= cols_b) {
    return;
  }
  double sum = 0.0;
  for (int k = 0; k < cols_a; ++k) {
    sum += a[row * cols_a + k] * b[k * cols_b + col];
  }
  c[row * cols_b + col] = sum;
}
)CLC";
}

inline const char *convolution3x3_source() {
  return R"CLC(
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void conv3(__global const double* image,
                    __global const double* filter,
                    __global double* output,
                    const int rows,
                    const int cols) {
  int row = get_global_id(0);
  int col = get_global_id(1);
  if (row >= rows || col >= cols) {
    return;
  }
  double sum = 0.0;
  for (int kr = 0; kr < 3; ++kr) {
    for (int kc = 0; kc < 3; ++kc) {
      int rr = row + kr - 1;
      int cc = col + kc - 1;
      rr = rr < 0 ? 0 : (rr >= rows ? rows - 1 : rr);
      cc = cc < 0 ? 0 : (cc >= cols ? cols - 1 : cc);
      sum += image[rr * cols + cc] * filter[kr * 3 + kc];
    }
  }
  output[row * cols + col] = sum;
}
)CLC";
}

inline OpenCLRuntime &runtime_instance() {
  static OpenCLRuntime runtime;
  return runtime;
}

inline cl_program matrix_program() {
  static ProgramHolder program(runtime_instance(), matrix_multiply_source());
  return program.get();
}

inline cl_program convolution3x3_program() {
  static ProgramHolder program(runtime_instance(), convolution3x3_source());
  return program.get();
}

inline std::vector<double> flatten(const MatrixD &matrix) {
  std::vector<double> values(matrix.row() * matrix.col(), 0.0);
  for (std::size_t r = 0; r < matrix.row(); r++) {
    for (std::size_t c = 0; c < matrix.col(); c++) {
      values[r * matrix.col() + c] = matrix[r][c];
    }
  }
  return values;
}

inline MatrixD unflatten(const std::vector<double> &values, std::size_t rows,
                         std::size_t cols) {
  MatrixD matrix(rows, cols, 0.0);
  for (std::size_t r = 0; r < rows; r++) {
    for (std::size_t c = 0; c < cols; c++) {
      matrix[r][c] = values[r * cols + c];
    }
  }
  return matrix;
}

} // namespace detail
#endif

inline std::string opencl_device_name() {
#ifdef LINEAR_ALGEBRA_HAS_OPENCL
  return detail::runtime_instance().device_name();
#else
  return "OpenCL not enabled";
#endif
}

inline MatrixD opencl_matrix_multiply(const MatrixD &lhs, const MatrixD &rhs) {
#ifdef LINEAR_ALGEBRA_HAS_OPENCL
  if (lhs.col() != rhs.row()) {
    throw(std::invalid_argument("error: OpenCL matrix multiplication size "
                                "mismatch"));
  }
  detail::OpenCLRuntime &runtime = detail::runtime_instance();
  cl_program program = detail::matrix_program();
  detail::UniqueKernel kernel = detail::make_kernel(program, "matmul");

  std::vector<double> a = detail::flatten(lhs);
  std::vector<double> b = detail::flatten(rhs);
  std::vector<double> c(lhs.row() * rhs.col(), 0.0);
  cl_context context = runtime.get_context();
  detail::UniqueMem a_buffer =
      detail::make_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                          sizeof(double) * a.size(), a.data(),
                          "failed to create OpenCL A buffer");
  detail::UniqueMem b_buffer =
      detail::make_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                          sizeof(double) * b.size(), b.data(),
                          "failed to create OpenCL B buffer");
  detail::UniqueMem c_buffer =
      detail::make_buffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * c.size(),
                          nullptr, "failed to create OpenCL C buffer");

  int rows_a = static_cast<int>(lhs.row());
  int cols_a = static_cast<int>(lhs.col());
  int cols_b = static_cast<int>(rhs.col());
  cl_mem a_mem = a_buffer.get();
  cl_mem b_mem = b_buffer.get();
  cl_mem c_mem = c_buffer.get();
  detail::check(clSetKernelArg(kernel.get(), 0, sizeof(cl_mem), &a_mem),
                "failed to set OpenCL matrix kernel arg 0");
  detail::check(clSetKernelArg(kernel.get(), 1, sizeof(cl_mem), &b_mem),
                "failed to set OpenCL matrix kernel arg 1");
  detail::check(clSetKernelArg(kernel.get(), 2, sizeof(cl_mem), &c_mem),
                "failed to set OpenCL matrix kernel arg 2");
  detail::check(clSetKernelArg(kernel.get(), 3, sizeof(int), &rows_a),
                "failed to set OpenCL matrix kernel arg 3");
  detail::check(clSetKernelArg(kernel.get(), 4, sizeof(int), &cols_a),
                "failed to set OpenCL matrix kernel arg 4");
  detail::check(clSetKernelArg(kernel.get(), 5, sizeof(int), &cols_b),
                "failed to set OpenCL matrix kernel arg 5");

  std::size_t global_size[2] = {lhs.row(), rhs.col()};
  detail::check(clEnqueueNDRangeKernel(runtime.get_queue(), kernel.get(), 2,
                                       nullptr, global_size, nullptr, 0,
                                       nullptr, nullptr),
                "failed to run OpenCL matrix kernel");
  detail::check(clEnqueueReadBuffer(runtime.get_queue(), c_mem, CL_TRUE, 0,
                                    sizeof(double) * c.size(), c.data(), 0,
                                    nullptr, nullptr),
                "failed to read OpenCL matrix result");

  return detail::unflatten(c, lhs.row(), rhs.col());
#else
  (void)lhs;
  (void)rhs;
  throw(std::runtime_error("error: OpenCL backend is not enabled"));
#endif
}

inline MatrixD opencl_convolve3x3(const MatrixD &image,
                                  const MatrixD &kernel3x3) {
#ifdef LINEAR_ALGEBRA_HAS_OPENCL
  if (kernel3x3.row() != 3 || kernel3x3.col() != 3) {
    throw(std::invalid_argument("error: OpenCL convolution requires a 3x3 "
                                "kernel"));
  }
  detail::OpenCLRuntime &runtime = detail::runtime_instance();
  cl_program program = detail::convolution3x3_program();
  detail::UniqueKernel kernel = detail::make_kernel(program, "conv3");

  std::vector<double> input = detail::flatten(image);
  std::vector<double> kernel_values = detail::flatten(kernel3x3);
  std::vector<double> output(image.row() * image.col(), 0.0);
  cl_context context = runtime.get_context();
  detail::UniqueMem image_buffer = detail::make_buffer(
      context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(double) * input.size(), input.data(),
      "failed to create OpenCL image buffer");
  detail::UniqueMem kernel_buffer = detail::make_buffer(
      context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(double) * kernel_values.size(), kernel_values.data(),
      "failed to create OpenCL convolution kernel buffer");
  detail::UniqueMem output_buffer = detail::make_buffer(
      context, CL_MEM_WRITE_ONLY, sizeof(double) * output.size(), nullptr,
      "failed to create OpenCL convolution output buffer");

  int rows = static_cast<int>(image.row());
  int cols = static_cast<int>(image.col());
  cl_mem image_mem = image_buffer.get();
  cl_mem kernel_mem = kernel_buffer.get();
  cl_mem output_mem = output_buffer.get();
  detail::check(clSetKernelArg(kernel.get(), 0, sizeof(cl_mem), &image_mem),
                "failed to set OpenCL convolution arg 0");
  detail::check(clSetKernelArg(kernel.get(), 1, sizeof(cl_mem), &kernel_mem),
                "failed to set OpenCL convolution arg 1");
  detail::check(clSetKernelArg(kernel.get(), 2, sizeof(cl_mem), &output_mem),
                "failed to set OpenCL convolution arg 2");
  detail::check(clSetKernelArg(kernel.get(), 3, sizeof(int), &rows),
                "failed to set OpenCL convolution arg 3");
  detail::check(clSetKernelArg(kernel.get(), 4, sizeof(int), &cols),
                "failed to set OpenCL convolution arg 4");

  std::size_t global_size[2] = {image.row(), image.col()};
  detail::check(clEnqueueNDRangeKernel(runtime.get_queue(), kernel.get(), 2,
                                       nullptr, global_size, nullptr, 0,
                                       nullptr, nullptr),
                "failed to run OpenCL convolution kernel");
  detail::check(clEnqueueReadBuffer(runtime.get_queue(), output_mem, CL_TRUE,
                                    0, sizeof(double) * output.size(),
                                    output.data(), 0, nullptr, nullptr),
                "failed to read OpenCL convolution result");

  return detail::unflatten(output, image.row(), image.col());
#else
  (void)image;
  (void)kernel3x3;
  throw(std::runtime_error("error: OpenCL backend is not enabled"));
#endif
}

} // namespace gpu_acceleration

#endif
