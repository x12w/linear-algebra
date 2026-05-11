#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>
#ifndef LINEAR_HPP
#define LINEAR_HPP

namespace linear_algebra {
template <typename T> class Vector {
private:
  using ValueType = T;
  using Iterator = ValueType *;
  std::allocator<ValueType> allo;
  std::size_t capacity;
  Iterator base, first_free, after_end;

public:
  std::size_t size() const { return first_free - base; }
  void resize(std::size_t target_size);
  std::size_t space() const { return capacity; }
  bool empty() const { return size() == 0; }
  void reserve(const std::size_t target_capacity);
  void check_size();
  void push_back(const ValueType &ele);
  void push_back(ValueType &&ele);
  ValueType pop_back();
  void insert(const std::size_t index, const ValueType &ele);
  void insert(const Iterator it, const ValueType &ele);
  void clear();
  void swap(Vector &another) {
    std::swap(capacity, another.capacity);
    std::swap(base, another.base);
    std::swap(first_free, another.first_free);
    std::swap(after_end, another.after_end);
  }

  // math methods
  Vector cross_product(const Vector &operand) const;

  // constructors and copy controllers
  Vector();
  Vector(std::size_t init_size);
  Vector(std::size_t init_size, const ValueType &ele);
  Vector(const Vector &another);
  Vector(Vector &&another);
  ~Vector();

  // reload operators
  Vector &operator=(Vector another);
  ValueType &operator[](std::size_t index) { return base[index]; }
  const ValueType &operator[](std::size_t index) const { return base[index]; }
  ValueType operator*(const Vector &another) const;
  Vector operator+(const Vector &operand) const;
  Vector &operator+=(const Vector &operand);
  Vector operator-(const Vector &operand) const;
  Vector &operator-=(const Vector &operand);
  Vector operator-() const;

  // friends
  template <typename U>
  friend std::ostream &operator<<(std::ostream &os, const Vector<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is, Vector<U> &operand);
};

// allocate space of target_size to the Vector,
// if the target_size is not bigger than the current capacity,
// do nothing
template <typename T>
void Vector<T>::reserve(const std::size_t target_capacity) {
  if (target_capacity <= capacity) {
    return;
  }
  // allocate new space
  auto new_base = allo.allocate(target_capacity);

  auto new_first_free = new_base;

  // move the data
  for (auto i = base; i < first_free; i++, new_first_free++) {
    allo.construct(new_first_free, std::move_if_noexcept(*i));
  }

  // destroy old objects and release the memory
  for (auto i = base; i < first_free; i++) {
    allo.destroy(i);
  }
  allo.deallocate(base, capacity);

  // update the pointers and status
  base = new_base;
  first_free = new_first_free;
  capacity = target_capacity;
  after_end = base + capacity;
}

// this method will forced modify the size,
// it may leat to data loss.
// the new objects will be initialized defaultly
template <typename T> void Vector<T>::resize(std::size_t target_size) {
  // if the size doesn't change
  if (target_size == size()) {
    return;
  }

  // check the capacity
  if (target_size > capacity) {
    reserve(target_size);
  }

  // if the target size is larger than the current size
  if (target_size > size()) {
    for (std::size_t index = size(); index < target_size;
         index++, first_free++) {
      allo.construct(base + index);
    }
    return;
  }
  // if the target size is less than the current size
  else {
    for (std::size_t index = size() - 1; index > target_size - 1;
         index--, first_free--) {
      allo.destroy(base + index);
    }
    return;
  }
}

template <typename T> void Vector<T>::check_size() {
  if (first_free == after_end) {
    reserve(capacity == 0 ? 1 : 2 * capacity);
  }
}

template <typename T> void Vector<T>::push_back(const ValueType &ele) {
  // check is there enough space
  check_size();

  allo.construct(first_free, ele);
  first_free++;
}

template <typename T> void Vector<T>::push_back(ValueType &&ele) {
  // check is there enough space
  check_size();

  allo.construct(first_free, std::move(ele));
  first_free++;
}

template <typename T> auto Vector<T>::pop_back() -> ValueType {
  auto res = std::move(*(base + size() - 1));
  allo.destroy(base + size() - 1);
  first_free--;
  return res;
}

// the insert method assume that the index/iterator is valid :)
template <typename T>
void Vector<T>::insert(const std::size_t index, const ValueType &ele) {
  // check is there enough space
  check_size();

  // construct the object at first_free
  // then update the first_free
  allo.construct(first_free);
  first_free++;

  // move the elements after the target element
  for (int i = size() - 1; i > index; i--) {
    *(base + i) = *(base + i - 1);
  }

  // insert the target element
  *(base + index) = ele;
}

template <typename T>
void Vector<T>::insert(const Iterator it, const ValueType &ele) {
  // check is there enough space
  check_size();

  // construct the object at first_free
  // then update the first_free
  allo.construct(first_free);
  first_free++;

  // move the elements after the target element
  for (auto i = first_free; i > it; i--) {
    *i = *(i - 1);
  }

  // insert the target element
  *it = ele;
}

template <typename T> void Vector<T>::clear() {
  for (; first_free > base; first_free--) {
    allo.destroy(first_free - 1);
  }
}

template <typename T>
auto Vector<T>::cross_product(const Vector &operand) const -> Vector {
  // check the vector size
  if ((size() != 3) || (operand.size() != 3)) {
    throw(std::runtime_error("error: to do the cross product, you should make "
                             "sure that the two vectors' size is 3"));
  }

  Vector res(3);

  res[0] = (*this)[1] * operand[2] - operand[1] * (*this)[2];
  res[1] = (*this)[2] * operand[0] - operand[2] * (*this)[0];
  res[2] = (*this)[0] * operand[1] - operand[0] * (*this)[1];

  return res;
}

template <typename T> Vector<T>::Vector() {
  base = allo.allocate(0);
  first_free = base;
  after_end = base;
  capacity = 0;
}

template <typename T> Vector<T>::Vector(std::size_t init_size) {
  base = allo.allocate(init_size);
  first_free = base + init_size;
  capacity = init_size;
  after_end = base + capacity;
  for (auto i = base; i < first_free; i++) {
    allo.construct(i);
  }
}

template <typename T>
Vector<T>::Vector(std::size_t init_size, const ValueType &ele) {
  base = allo.allocate(init_size);
  first_free = base + init_size;
  capacity = init_size;
  after_end = base + capacity;
  for (auto i = base; i < first_free; i++) {
    allo.construct(i, ele);
  }
}

template <typename T> Vector<T>::Vector(const Vector &another) {
  base = allo.allocate(another.size());
  first_free = base;
  capacity = another.size();
  after_end = base + capacity;

  for (auto i = another.base; i < another.first_free; i++, first_free++) {
    allo.construct(first_free, *i);
  }
}

template <typename T> Vector<T>::Vector(Vector &&another) {
  base = allo.allocate(another.size());
  first_free = base;
  capacity = another.size();
  after_end = base + capacity;

  for (auto i = another.base; i < another.first_free; i++, first_free++) {
    allo.construct(first_free, std::move_if_noexcept(*i));
  }
}

template <typename T> Vector<T>::~Vector() {
  for (auto i = base; i < first_free; i++) {
    allo.destroy(i);
  }
  allo.deallocate(base, capacity);
}

template <typename T> Vector<T> &Vector<T>::operator=(Vector another) {
  swap(another);
  return *this;
}

template <typename T>
auto Vector<T>::operator*(const Vector &another) const -> ValueType {
  if (size() != another.size()) {
    throw(std::runtime_error(
        "error:the vector dot product need the two vector has same size"));
  }

  ValueType res = 0;
  for (std::size_t i = 0; i < size(); i++) {
    res += (*this)[i] * another[i];
  }
  return res;
}

template <typename T>
auto Vector<T>::operator+(const Vector &operand) const -> Vector {
  // check the vectors' size
  if (size() != operand.size()) {
    throw(std::runtime_error("error: to do a vector add, you should make sure "
                             "that the two vectors' size equal"));
  }

  Vector res(*this);

  for (std::size_t index = 0; index < res.size(); index++) {
    res[index] += operand[index];
  }

  return res;
}

template <typename T>
auto Vector<T>::operator+=(const Vector &operand) -> Vector & {
  *this = (*this) + operand;
  return *this;
}

template <typename T>
auto Vector<T>::operator-(const Vector &operand) const -> Vector {
  // check the vectors' size
  if (size() != operand.size()) {
    throw(std::runtime_error("error: to do a vector subtraction, you should "
                             "make sure that the two vectors' size equal"));
  }

  Vector res(*this);

  for (std::size_t index = 0; index < res.size(); index++) {
    res[index] -= operand[index];
  }

  return res;
}

template <typename T>
auto Vector<T>::operator-=(const Vector &operand) -> Vector & {
  *this = (*this) - operand;
  return *this;
}

template <typename T> auto Vector<T>::operator-() const -> Vector {
  Vector res(size());
  for (std::size_t index = 0; index < size(); index++) {
    res[index] = -(*this)[index];
  }
  return res;
}

// friends

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vector<T> &operand) {
  for (std::size_t index = 0; index < operand.size(); index++) {
    os << operand[index] << " ";
  }

  return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, Vector<T> &operand) {
  for (std::size_t index = 0; index < operand.size(); index++) {
    is >> operand[index];
  }
  return is;
}

// to implement the class matrix, the type T should reload the following
// operators:
//+, -, *, /, +=, -=, *=, /=, -(the unary operator), ==. !=, <<, >>(input and
// output)
template <typename T> class Matrix {
private:
  using ValueType = T;
  Vector<Vector<ValueType>> matrix;

public:
  std::size_t row() const { return matrix.size(); }

  std::size_t col() const {
    // determine whether the matrix is empty
    if (matrix.empty()) {
      return 0;
    }

    return matrix[0].size();
  }

  // make the matrix bigger, the method will ignore the operations making the
  // matrix smaller the new space will be filld with 0
  void resize(std::size_t row_size, std::size_t col_size);
  // forced resize the matrix, it would lead to data loss when make the matrix
  // smaller the smaller matrix will take the top-left part of the origin matrix
  void forced_resize(std::size_t row_size, std::size_t col_size);
  // add row/col, the version without parameters will fill the new place with 0
  // the added row will be at the bottom, the added col will be at the most
  // right
  void add_row();
  void add_row(const Vector<ValueType> &new_row);
  void add_row(Vector<ValueType> &&new_row);
  void add_col();
  void add_col(const Vector<ValueType> &new_row);
  void clear();
  Vector<ValueType> fetch_row(std::size_t row_index) const;
  Vector<ValueType> fetch_col(std::size_t col_index) const;
  Matrix get_transpose() const;
  void transpose();

  // constructors and copy controllers
  Matrix();
  Matrix(std::size_t row_size, std::size_t col_size, const ValueType &ele = 0);
  ~Matrix() = default;

  // reload operators
  Matrix operator+(const Matrix &operand) const;
  Matrix &operator+=(const Matrix &operand);
  Matrix operator-(const Matrix &operand) const;
  Matrix &operator-=(const Matrix &operand);
  Matrix operator*(const Matrix &operand) const;
  Matrix &operator*=(const Matrix &operand);
  Matrix operator*(const ValueType &operand) const;
  Matrix &operator*=(const ValueType &operand);
  Matrix operator/(const ValueType &operand) const;
  Matrix &operator/=(const ValueType &operand);
  Matrix operator-() const;
  Matrix operator^(const Matrix &operand) const;
  Vector<ValueType> &operator[](std::size_t index) { return matrix[index]; }
  const Vector<ValueType> &operator[](std::size_t index) const {
    return matrix[index];
  }

  // friends
  template <typename U>
  friend std::ostream &operator<<(std::ostream &os, const Matrix<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is, Matrix<U> &operand);
};

template <typename T>
auto Matrix<T>::fetch_row(std::size_t row_index) const -> Vector<ValueType> {
  return matrix[row_index];
}

template <typename T>
auto Matrix<T>::fetch_col(std::size_t col_index) const -> Vector<ValueType> {
  Vector<ValueType> res;
  for (int i = 0; i < matrix.size(); i++) {
    res.push_back(matrix[i][col_index]);
  }
  return res;
}

template <typename T> void Matrix<T>::add_row() {
  matrix.push_back(Vector<ValueType>(col, 0));
}

template <typename T>
void Matrix<T>::add_row(const Vector<ValueType> &new_row) {
  if (col() != new_row.size()) {
    throw(std::runtime_error(
        "error:to add a row to the Matrix, you should make sure that the "
        "Matrix's col_size is equal to the row's size"));
  }

  matrix.push_back(new_row);
}

template <typename T> void Matrix<T>::add_row(Vector<ValueType> &&new_row) {
  if (col() != new_row.size()) {
    throw(std::runtime_error(
        "error:to add a row to the Matrix, you should make sure that the "
        "Matrix's col_size is equal to the row's size"));
  }

  matrix.push_back(std::move(new_row));
}

template <typename T> void Matrix<T>::add_col() {
  for (int i = 0; i < matrix.size(); i++) {
    matrix[i].push_back(0);
  }
}

template <typename T>
void Matrix<T>::add_col(const Vector<ValueType> &new_col) {
  if (row() != new_col.size()) {
    throw(std::runtime_error(
        "error:to add a col to the Matrix, you should make sure that the "
        "Matrix's row_size is equal to the col's size"));
  }

  for (int i = 0; i < matrix.size(); i++) {
    matrix[i].push_back(std::move(new_col[i]));
  }
}

template <typename T> void Matrix<T>::clear() { matrix.clear(); }

template <typename T>
void Matrix<T>::resize(std::size_t row_size, std::size_t col_size) {
  // if row/col become smaller or the matrix doesn't change
  if (row_size < row() || col_size < col() ||
      (row_size == row() && col_size == col())) {
    return;
  }

  // the matrix become bigger(both the row and col)
  for (std::size_t row_index = 0; row_index < matrix.size(); row_index++) {
    matrix[row_index].resize(col_size);
  }
  for (std::size_t index = 0; index < row_size; index++) {
    matrix.push_back(Vector<ValueType>(col_size));
  }
}

template <typename T>
void Matrix<T>::forced_resize(std::size_t row_size, std::size_t col_size) {
  // if the matrix become larger
  if (row_size >= row() && col_size >= col()) {
    // if the matrix doesn't change
    if (row_size == row() && col_size == col()) {
      return;
    }

    // the matrix become bigger(both the row and col)
    for (std::size_t row_index = 0; row_index < matrix.size(); row_index++) {
      matrix[row_index].resize(col_size);
    }
    for (std::size_t index = 0; index < row_size; index++) {
      matrix.push_back(Vector<ValueType>(col_size));
    }
    return;
  }

  // if the matrix's row/col(or both) become smaller
  if (row_size < row() && col_size < col()) {
    matrix.resize(row_size);
    for (std::size_t index = 0; index < matrix.size(); index++) {
      matrix[index].resize(col_size);
    }
    return;
  }
  if (row_size < row()) {
    matrix.resize(row_size);
    for (std::size_t index = 0; index < matrix.size(); index++) {
      matrix[index].resize(col_size);
    }
    return;
  }
  if (col_size < col()) {
    for (std::size_t index = 0; index < matrix.size(); index++) {
      matrix.resize(col_size);
    }
    for (std::size_t index = 0; index < row_size; index++) {
      matrix.push_back(Vector<ValueType>(col_size, 0));
    }
  }
}

template <typename T> auto Matrix<T>::get_transpose() const -> Matrix {
  Matrix res(col(), 0);

  for (std::size_t index = 0; index < matrix.size(); index++) {
    res.add_col(matrix[index]);
  }

  return res;
}

template <typename T> Matrix<T>::Matrix() : matrix() {}

template <typename T>
Matrix<T>::Matrix(std::size_t row_size, std::size_t col_size,
                  const ValueType &ele)
    : matrix(row_size, Vector<ValueType>(col_size, ele)) {}

template <typename T>
auto Matrix<T>::operator+(const Matrix &operand) const -> Matrix {
  // check the whether the col and rol are both equal
  if ((row() != operand.row()) || (col() != operand.col())) {
    throw(std::runtime_error("error: to do a matrix add, you should make sure "
                             "the two matrixes have equal size"));
  }

  Matrix res(*this);

  std::cout << res << std::endl;

  for (std::size_t index = 0; index < operand.row(); index++) {
    res[index] += operand[index];
  }

  return res;
}

template <typename T>
auto Matrix<T>::operator+=(const Matrix &operand) -> Matrix & {
  *this = (*this) + operand;
  return *this;
}

template <typename T>
auto Matrix<T>::operator-(const Matrix &operand) const -> Matrix {
  // check the matrix size
  if ((row() != operand.row()) || (col() != operand.col())) {
    throw(std::runtime_error("error: to do a matrix subtraction, you should "
                             "make sure the matrixes have equal size"));
  }

  Matrix res(*this);

  for (std::size_t index = 0; index < res.row(); index++) {
    res[index] -= operand[index];
  }

  return res;
}

template <typename T>
auto Matrix<T>::operator-=(const Matrix &operand) -> Matrix & {
  *this = (*this) - operand;
  return *this;
}

template <typename T>
auto Matrix<T>::operator*(const Matrix &operand) const -> Matrix {
  // check the matrix size
  if (col() != operand.row()) {
    throw(std::runtime_error(
        "error: to do a matrix multiplication, you should make sure the first "
        "matrix's col is equal to the second matrix's row"));
  }

  Matrix res(row(), operand.col());

  for (std::size_t row_index = 0; row_index < res.row(); row_index++) {
    for (std::size_t col_index = 0; col_index < res.col(); col_index++) {
      res[row_index][col_index] =
          (*this)[row_index] * operand.fetch_col(col_index);
    }
  }

  return res;
}

template <typename T>
auto Matrix<T>::operator*=(const Matrix &operand) -> Matrix & {
  *this = (*this) * operand;
  return *this;
}

template <typename T> auto Matrix<T>::operator-() const -> Matrix {
  Matrix res(*this);

  for (std::size_t index; index < res.row(); index++) {
    res[index] = -res[index];
  }

  return res;
}

// friends
template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &operand) {
  for (std::size_t index = 0; index < operand.row(); index++) {
    os << operand.matrix[index] << std::endl;
  }
  return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, Matrix<T> &operand) {
  for (std::size_t index = 0; index < operand.matrix.size(); index++) {
    is >> operand.matrix[index];
  }
  return is;
}

} // namespace linear_algebra

#endif
