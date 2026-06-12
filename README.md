# Linear Algebra

本项目是一个 C++17 线性代数计算库实训项目，目标是实现矩阵、向量、基础数值类型和高精度数值类型，并提供常见线性代数运算接口。项目当前覆盖任务一到任务四的核心要求，包括矩阵与向量基础运算、行列式、逆矩阵、秩、行最简形、各类范数、条件数、线性方程组求解、最小二乘、主特征值/特征向量、分块矩阵乘法、Strassen 矩阵乘法、图像卷积和图像特征分析。

## 项目目标

- 使用类、模板、继承和运算符重载实现一个可复用的线性代数库。
- 支持实数、复数、有理数、高精度整数和高精度浮点数等不同数值类型。
- 提供矩阵和向量的基础代数运算。
- 提供任务二要求的向量范数、矩阵范数和条件数计算。
- 提供任务三要求的方阵线性方程组求解、非方阵最小二乘、主特征值/特征向量、分块矩阵乘法和 Strassen 矩阵乘法。
- 提供任务四要求的卷积、Sobel 边缘检测、图像特征统计和 PGM/OpenCV 图像读写。
- 提供菜单演示、样例数据、多线程优化和可选 OpenCL GPU 加速对比。
- 对维度不匹配、非方阵求行列式/逆矩阵、奇异矩阵求逆等非法操作抛出异常。

## 目录结构

```text
.
├── CMakeLists.txt
├── include
│   ├── basic_algebra.hpp
│   ├── gpu_acceleration.hpp
│   ├── image_processing.hpp
│   ├── linear_algebra.hpp
│   └── utils.hpp
├── data
│   ├── eigen_matrix.txt
│   ├── least_squares_A.txt
│   ├── linear_system_A.txt
│   ├── matrix_a.txt
│   ├── perf_matrix_a.txt
│   ├── sample_image.pgm
│   └── vector_a.txt
└── src
    ├── menu_demo.cpp
    └── final_demo.cpp
```

说明：

- `include/basic_algebra.hpp`：基础代数类型，包括复数、有理数、高精度整数和高精度浮点数。
- `include/gpu_acceleration.hpp`：可选 OpenCL 矩阵乘法和 3x3 卷积加速。
- `include/linear_algebra.hpp`：向量、矩阵和线性代数运算。
- `include/image_processing.hpp`：任务四图像卷积、Sobel 边缘检测和图像特征统计。
- `include/utils.hpp`：基础动态数组工具类。
- `src/final_demo.cpp`：最终阶段演示程序，用于展示主要接口效果。
- `src/menu_demo.cpp`：交互式菜单演示程序，可用数字选择展示操作。
- `data/`：菜单演示所需的矩阵、向量和 PGM 图像样例数据。

## 构建与运行

```bash
cmake -S . -B build
cmake --build build
./build/test
./build/menu_demo
```

项目使用 C++17，库本身以头文件形式提供，CMake 中定义了 `BasicAlgebra`、`LinearAlgebra`、`ImageProcessing`、`GPUAcceleration` 和 `Utils` 五个 interface library。

交互式演示程序 `menu_demo` 支持数字菜单：

```text
1. 向量运算
2. 矩阵基础运算
3. 线性方程组求解
4. 最小二乘拟合
5. 范数、条件数和特征值
6. 矩阵乘法性能对比
7. 图像卷积和边缘特征分析
8. 基础数值类型演示
9. 优化前后计算用时对比
0. 退出
```

`menu_demo` 会自动定位项目根目录下的 `data/`，因此既可以在项目根目录运行 `./build/menu_demo`，也可以进入 `build/` 后运行 `./menu_demo`。

菜单演示默认读取 `data/` 下的样例文件：

- `vector_a.txt`、`vector_b.txt`：三维向量，用于加减、点积、叉积和范数。
- `matrix_a.txt`、`matrix_b.txt`：3x3 矩阵，用于矩阵加减乘、转置、行列式和逆矩阵。
- `linear_system_A.txt`、`linear_system_b.txt`：线性方程组 `Ax=b`。
- `least_squares_A.txt`、`least_squares_b.txt`：直线拟合最小二乘样例。
- `eigen_matrix.txt`：对称矩阵，用于范数、条件数和特征值展示。
- `perf_matrix_a.txt`、`perf_matrix_b.txt`：矩阵乘法性能对比样例。
- `sample_image.pgm`：ASCII PGM 灰度图，用于卷积、Sobel 边缘检测和特征统计。

第 9 项会自动生成 512x512 确定性矩阵，并对比优化前列向量临时构造乘法、优化后 `operator*`、分块乘法、Strassen、多线程分块乘法和可选 OpenCL 后端的计算用时。OpenCL 路径会先做一次同规模预热，正式计时不包含首次 runtime/program 初始化。

## Nix 与 Direnv

项目提供 `shell.nix` 和 `.envrc`，用于获得一致的 C++/CMake/OpenCV/OpenCL 开发环境：

```bash
direnv allow
cmake -S . -B build-nix
cmake --build build-nix
./build-nix/test
```

也可以直接使用：

```bash
nix-shell --run 'cmake -S . -B build-nix && cmake --build build-nix'
```

OpenCV 和 OpenCL 在当前阶段作为可选依赖进入环境和 CMake 探测；核心卷积与 PGM 演示使用项目自身的矩阵库实现，保证无 OpenCV/OpenCL 时仍可构建运行。若使用 Nix/direnv 环境构建，演示程序会自动处理项目根目录下的测试 JPG 图片，并启用 OpenCL 加速路径。

OpenCL 后端必须使用启用了 OpenCL 的构建产物。普通 `build/` 如果是在系统缺少 OpenCL headers/loader 时配置的，会显示 `OpenCL backend is not enabled`。推荐直接运行：

```bash
nix-shell --run 'cmake -S . -B build-nix && cmake --build build-nix && ./build-nix/menu_demo'
```

如果单独执行 `./build-nix/menu_demo` 仍出现 `failed to query OpenCL platforms`，说明当前 shell 没有可用的 OpenCL ICD 平台；请在 `nix-shell --run` 或 `direnv allow` 后的环境中运行。

## 命名空间

项目主要使用以下命名空间：

```cpp
namespace basic_algebra;
namespace linear_algebra;
namespace image_processing;
namespace gpu_acceleration;
```

使用示例：

```cpp
#include <basic_algebra.hpp>
#include <linear_algebra.hpp>

using namespace basic_algebra;
using namespace linear_algebra;
```

## 基础代数接口

### `ComplexNumber<T>`

模板复数类型，支持：

- 构造：`ComplexNumber<T>()`、`ComplexNumber<T>(real, imaginary)`
- 运算：`+`、`-`、`*`、`/`
- 复合赋值：`+=`、`-=`、`*=`、`/=`
- 一元负号：`-x`
- 比较：`==`、`!=`
- 流输入输出：`>>`、`<<`

### `RationalNumber<T>`

有理数类型，默认底层整数类型为 `int`。

支持：

- 构造：`RationalNumber<T>()`、`RationalNumber<T>(molecular, denominator)`
- 自动约分，分母保持为正数
- 运算：`+`、`-`、`*`、`/`
- 复合赋值：`+=`、`-=`、`*=`、`/=`
- 比较：`==`、`!=`、`<`、`<=`、`>`、`>=`
- 流输入输出：`>>`、`<<`

### `BigInteger<>`

高精度整数类型，用于处理超过内置整数范围的有符号整数。

支持：

- 构造：`BigInteger<>()`、`BigInteger<>(long long)`、`BigInteger<>("123456789")`
- 运算：`+`、`-`、`*`、`/`、`%`
- 复合赋值：`+=`、`-=`、`*=`、`/=`、`%=`
- 比较：`==`、`!=`、`<`、`<=`、`>`、`>=`
- 流输入输出：`>>`、`<<`

示例：

```cpp
BigInteger<> a("12345678901234567890");
BigInteger<> b("9876543210");
std::cout << a + b << std::endl;
```

### `HighPrecisionNumber<>`

高精度浮点/定点小数类型，内部使用 `BigInteger<>` 保存放大后的整数，并记录小数位数。

支持：

- 构造：`HighPrecisionNumber<>()`
- 整数构造：`HighPrecisionNumber<>(long long)`
- 字符串构造：`HighPrecisionNumber<>("1.2345")`
- 非 `explicit` 浮点构造：`HighPrecisionNumber<>(float)`、`HighPrecisionNumber<>(double)`
- `set_default_division_precision(precision)`：设置除法结果默认保留的小数位数
- `default_division_precision()`：读取当前默认除法精度
- 运算：`+`、`-`、`*`、`/`
- 复合赋值：`+=`、`-=`、`*=`、`/=`
- 比较：`==`、`!=`、`<`、`<=`、`>`、`>=`
- 流输入输出：`>>`、`<<`

示例：

```cpp
HighPrecisionNumber<> x = 1.25;
HighPrecisionNumber<> y("2.5");
HighPrecisionNumber<>::set_default_division_precision(50);
std::cout << x + y << std::endl;
```

## 线性代数接口

### `MatrixBase<T>`

矩阵抽象基类，用于体现继承和多态。

接口：

- `virtual std::size_t row() const = 0`
- `virtual std::size_t col() const = 0`
- 虚析构函数

### `Vector<T>`

动态向量类型。

基础接口：

- `size()`：元素个数
- `resize(size)`：调整大小
- `reserve(capacity)`：预留空间
- `push_back(value)`：追加元素
- `pop_back()`：弹出末尾元素
- `insert(index, value)`：插入元素
- `clear()`：清空
- `operator[]`：随机访问

向量运算：

- `operator+(Vector)`：向量加法
- `operator-(Vector)`：向量减法
- `operator-()`：向量取负
- `operator*(Vector)`：点积
- `operator*(scalar)`：数乘
- `operator/(scalar)`：数除
- `cross_product(Vector)`：三维叉积

任务二范数接口：

- `norm(p)`：p-范数，要求 `p >= 1`
- `infinity_norm()`：无穷范数

示例：

```cpp
Vector<double> v(3);
v[0] = 3;
v[1] = 4;
v[2] = -12;

std::cout << v.norm(1) << std::endl;
std::cout << v.norm(2) << std::endl;
std::cout << v.infinity_norm() << std::endl;
```

### `Matrix<T>`

矩阵类型，继承自 `MatrixBase<T>`。

基础接口：

- `row()`：矩阵行数
- `col()`：矩阵列数
- `resize(row, col)`：扩展矩阵大小
- `forced_resize(row, col)`：强制调整矩阵大小
- `add_row()` / `add_row(vector)`：添加行
- `add_col()` / `add_col(vector)`：添加列
- `fetch_row(index)`：获取指定行
- `fetch_col(index)`：获取指定列
- `clear()`：清空矩阵
- `operator[]`：按行访问

矩阵运算：

- `operator+(Matrix)`：矩阵加法
- `operator-(Matrix)`：矩阵减法
- `operator*(Matrix)`：矩阵乘法
- `operator*(Vector)`：矩阵乘向量
- `operator*(scalar)`：矩阵数乘
- `operator/(scalar)`：矩阵数除
- `operator-()`：矩阵取负
- `operator==` / `operator!=`：矩阵比较

线性代数接口：

- `get_transpose()`：返回转置矩阵
- `transpose()`：原地转置
- `determinant()`：计算方阵行列式
- `inverse()`：计算逆矩阵
- `adjugate()`：计算伴随矩阵
- `row_echelon_form()`：计算行阶梯形
- `reduced_row_echelon_form()`：计算行最简形
- `rank()`：计算矩阵秩

任务二范数与条件数接口：

- `frobenius_norm()`：Frobenius 范数
- `row_sum_norm()`：行和范数
- `col_sum_norm()`：列和范数
- `spectral_norm(max_iterations, tolerance)`：谱范数，使用幂迭代近似最大奇异值
- `condition_number()`：条件数，当前使用行和范数计算 `||A|| * ||A^{-1}||`

任务三接口：

- `solve(b)`：方阵线性方程组 `Ax=b` 求解，使用带主元选择的高斯消元。
- `least_squares(b)`：非方阵最小二乘解，浮点类型默认使用 QR 分解，非浮点精确类型使用正规方程。
- `qr_decomposition(tolerance)`：改进 Gram-Schmidt QR 分解，返回 `Q` 和 `R`。
- `least_squares_qr(b, tolerance)`：使用 QR 分解求更稳定的最小二乘解。
- `dominant_eigenpair(max_iterations, tolerance)`：幂迭代求主特征值和对应特征向量。
- `eigenvalues_qr(max_iterations, tolerance)`：QR 迭代求实方阵全部特征值近似。
- `block_multiply(matrix, block_size)`：分块矩阵乘法，用于和普通三重循环矩阵乘法对比。
- `strassen_multiply(matrix, threshold)`：Strassen 矩阵乘法，自动补零到 2 的幂并在小规模时回退到分块乘法。
- `parallel_block_multiply(matrix, block_size, thread_count)`：多线程分块矩阵乘法。
- `parallel_frobenius_norm(thread_count)`：多线程 Frobenius 范数。

GPU 加速接口位于 `gpu_acceleration.hpp`：

- `opencl_compiled()`：当前构建是否启用 OpenCL。
- `opencl_device_name()`：返回 OpenCL 设备名称。
- `opencl_matrix_multiply(lhs, rhs)`：OpenCL `double` 矩阵乘法。
- `opencl_convolve3x3(image, kernel)`：OpenCL `double` 3x3 卷积。

文件接口：

- `save_to_file(path)`：保存矩阵到文本文件
- `Matrix<T>::load_from_file(path)`：从文本文件读取矩阵

示例：

```cpp
Matrix<double> a(2, 2);
a[0][0] = 1;
a[0][1] = 2;
a[1][0] = 3;
a[1][1] = 4;

std::cout << a.determinant() << std::endl;
std::cout << a.inverse() << std::endl;
std::cout << a.frobenius_norm() << std::endl;
std::cout << a.condition_number() << std::endl;
```

方程组与特征值示例：

```cpp
Matrix<double> a(2, 2);
a[0][0] = 2;
a[0][1] = 1;
a[1][0] = 1;
a[1][1] = 2;

Vector<double> b(2);
b[0] = 3;
b[1] = 3;

std::cout << a.solve(b) << std::endl;
auto eigen = a.dominant_eigenpair();
std::cout << eigen.first << std::endl;
std::cout << eigen.second << std::endl;
```

## 范数说明

向量 p-范数：

```text
||x||_p = (sum |x_i|^p)^(1/p), p >= 1
```

向量无穷范数：

```text
||x||_inf = max |x_i|
```

矩阵 Frobenius 范数：

```text
||A||_F = sqrt(sum_i sum_j |a_ij|^2)
```

矩阵行和范数：

```text
||A||_inf = max_i sum_j |a_ij|
```

矩阵列和范数：

```text
||A||_1 = max_j sum_i |a_ij|
```

谱范数：

```text
||A||_2 = 最大奇异值
```

当前实现通过对 `A^T A` 进行幂迭代，近似计算最大特征值，再开平方得到最大奇异值。

条件数：

```text
cond(A) = ||A|| * ||A^{-1}||
```

当前 `condition_number()` 使用行和范数。如果矩阵不是方阵或不可逆，返回正无穷 `infinity`。

## 任务三数学说明

方阵线性方程组：

```text
Ax = b
```

当前使用带主元选择的高斯消元，将矩阵化为上三角形式后回代求解。若矩阵奇异，则不存在唯一解并抛出异常。

数值稳定性：

```text
浮点数主元判断使用 epsilon 容差，而不是直接比较 value == 0
```

库内部通过类型相关的 `NumericTraits<T>` 统一近零判断。`float`、`double`、`long double` 和 `std::complex<T>` 使用基于机器精度的容差；整数、有理数和高精度数默认使用精确零判断。

最小二乘：

```text
min ||Ax - b||_2
A^T A x = A^T b
```

当方程组为过定约束时，`least_squares()` 求使残差平方和最小的解，可用于线性回归和多项式拟合。浮点矩阵默认走 QR 分解，避免正规方程中 `A^T A` 放大条件数；有理数等精确类型仍可使用正规方程路径。

主特征值与特征向量：

```text
Ax = lambda x
```

`dominant_eigenpair()` 使用幂迭代法求绝对值最大的特征值及其单位化特征向量。该能力可支撑 PCA 中协方差矩阵主方向的计算。

全部特征值：

```text
A_k = Q_k R_k
A_{k+1} = R_k Q_k
```

`eigenvalues_qr()` 使用未带位移的 QR 迭代，适合课程演示和实对称矩阵场景。对于存在复特征值或收敛较慢的矩阵，结果仍是近似值。

分块矩阵乘法：

```text
C_ij += A_ik * B_kj
```

`block_multiply()` 按块遍历矩阵，减少大矩阵乘法中的缓存失效，可与普通 `operator*` 的三重循环结果做一致性和性能对比。

Strassen 矩阵乘法：

```text
普通矩阵乘法复杂度: O(n^3)
Strassen 复杂度: O(n^log2(7)) ≈ O(n^2.807)
```

`strassen_multiply()` 对任意可乘矩阵先补零到 2 的幂，递归使用 7 次子矩阵乘法替代 8 次子矩阵乘法。为了控制递归开销，小规模子问题会回退到分块矩阵乘法。该方法适合中大型稠密矩阵；小矩阵或尺寸补零过多时可能不如分块乘法。

多线程优化：

```text
parallel_block_multiply() 按结果矩阵行块拆分任务
parallel_frobenius_norm() 按行拆分平方和
```

多线程接口默认使用 `std::thread::hardware_concurrency()`，也支持手动指定线程数。小矩阵会自动回退到单线程实现，避免线程创建成本超过计算收益。

GPU 加速：

- 已实现可选 OpenCL 后端，当前覆盖 `Matrix<double>` 矩阵乘法和 3x3 图像卷积。
- OpenCL 构建由 CMake 自动探测；无 OpenCL 时演示会跳过 GPU 路径并继续使用 CPU。
- Nix 环境提供 OpenCL headers、ICD loader 和 POCL，同时也可使用系统 NVIDIA OpenCL 平台。
- 首次 GPU 调用会完成 OpenCL 运行时和 program 初始化；后续调用复用 runtime/program，但仍包含 kernel 创建、buffer 分配和数据传输开销，单次小规模运行不一定比 CPU 快。
- `BigInteger`、`RationalNumber`、`HighPrecisionNumber` 等自定义精确数值类型不适合直接 GPU 化；GPU 后端主要面向 `float/double` 稠密矩阵和图像算子。

## 任务四图像卷积

`image_processing.hpp` 将灰度图像表示为 `Matrix<double>`，并提供：

- `convolve2d(image, kernel)`：二维卷积，边界使用 clamp 策略。
- `gaussian_kernel_3x3()`：3x3 高斯平滑核。
- `sobel_x_kernel()` / `sobel_y_kernel()`：Sobel 水平和垂直梯度核。
- `sobel_edges(image)`：高斯平滑后计算 Sobel 梯度幅值。
- `threshold(image, threshold)`：二值化边缘图。
- `normalize_to_byte_range(image)`：归一化到 0-255。
- `analyze_features(image, edge_threshold)`：统计最小值、最大值、均值、方差和边缘密度。
- `save_pgm(image, path)` / `load_pgm(path)`：ASCII PGM 图像读写。
- `load_grayscale_image(path)` / `save_grayscale_image(image, path)`：OpenCV 可用时支持 JPG/PNG 等常见格式，否则回退到 PGM。

最终演示会生成合成灰度图，并输出：

```text
output/task4_original.pgm
output/task4_blur.pgm
output/task4_sobel.pgm
output/task4_edges.pgm
```

这些文件可用常见图像查看器或 OpenCV/Python 读取，用于报告展示卷积、平滑、边缘检测和特征统计结果。

如果项目根目录存在 `istockphoto-184276818-612x612.jpg` 且当前构建启用了 OpenCV，演示还会输出真实图片处理结果：

```text
output/task4_real_gray.png
output/task4_real_blur.png
output/task4_real_sobel.png
output/task4_real_edges.png
```

当前测试图像的真实图片分析会打印尺寸、Sobel 边缘均值、方差和边缘密度。

## 异常处理

以下情况会抛出异常：

- 矩阵或向量维度不匹配。
- 非方阵求行列式、逆矩阵或伴随矩阵。
- 奇异矩阵求逆。
- 奇异方阵求唯一线性方程组解。
- 最小二乘右端向量维度不匹配。
- 非方阵求特征值。
- 分块矩阵乘法中块大小为 0。
- 卷积核为空或卷积核尺寸不是奇数。
- PGM 文件打开失败或格式不合法。
- 除数为 0。
- p-范数中 `p < 1`。
- 文件读取或写入失败。

## 当前限制

- 谱范数为迭代近似结果，不是符号精确计算。
- 主特征值使用幂迭代；全部特征值使用未带位移的 QR 迭代，复杂矩阵可能收敛较慢。
- QR 最小二乘当前要求 `row >= col` 且矩阵列满秩。
- `HighPrecisionNumber<>` 使用定点小数模型，除法精度可配置，默认保留 30 位小数。
- OpenCL GPU 后端当前覆盖 `double` 矩阵乘法和 3x3 卷积，尚未覆盖 QR、消元、高精度数值类型或通用卷积核。
- 任务四核心算法仍以灰度矩阵为基础；OpenCV 分支已支持常见图片格式的灰度读取和 PNG 输出。
