# Linear Algebra

本项目是一个 C++17 线性代数计算库实训项目，目标是实现矩阵、向量、基础数值类型和高精度数值类型，并提供常见线性代数运算接口。项目当前覆盖任务一和任务二的核心要求，包括矩阵与向量基础运算、行列式、逆矩阵、秩、行最简形、各类范数和条件数。

## 项目目标

- 使用类、模板、继承和运算符重载实现一个可复用的线性代数库。
- 支持实数、复数、有理数、高精度整数和高精度浮点数等不同数值类型。
- 提供矩阵和向量的基础代数运算。
- 提供任务二要求的向量范数、矩阵范数和条件数计算。
- 对维度不匹配、非方阵求行列式/逆矩阵、奇异矩阵求逆等非法操作抛出异常。

## 目录结构

```text
.
├── CMakeLists.txt
├── include
│   ├── basic_algebra.hpp
│   ├── linear_algebra.hpp
│   └── utils.hpp
└── test.cpp
```

说明：

- `include/basic_algebra.hpp`：基础代数类型，包括复数、有理数、高精度整数和高精度浮点数。
- `include/linear_algebra.hpp`：向量、矩阵和线性代数运算。
- `include/utils.hpp`：基础动态数组工具类。
- `test.cpp`：本地演示程序，用于展示主要接口效果。

## 构建与运行

```bash
cmake -S . -B build
cmake --build build
./build/test
```

项目使用 C++17，库本身以头文件形式提供，CMake 中定义了 `BasicAlgebra`、`LinearAlgebra` 和 `Utils` 三个 interface library。

## 命名空间

项目主要使用两个命名空间：

```cpp
namespace basic_algebra;
namespace linear_algebra;
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
- 运算：`+`、`-`、`*`、`/`
- 复合赋值：`+=`、`-=`、`*=`、`/=`
- 比较：`==`、`!=`、`<`、`<=`、`>`、`>=`
- 流输入输出：`>>`、`<<`

示例：

```cpp
HighPrecisionNumber<> x = 1.25;
HighPrecisionNumber<> y("2.5");
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

## 异常处理

以下情况会抛出异常：

- 矩阵或向量维度不匹配。
- 非方阵求行列式、逆矩阵或伴随矩阵。
- 奇异矩阵求逆。
- 除数为 0。
- p-范数中 `p < 1`。
- 文件读取或写入失败。

## 当前限制

- 谱范数为迭代近似结果，不是符号精确计算。
- `HighPrecisionNumber<>` 使用定点小数模型，除法默认保留 30 位小数。
- 矩阵算法以清晰实现为主，没有针对大规模矩阵做性能优化。
