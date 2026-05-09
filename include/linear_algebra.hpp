#include <algorithm>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <memory>
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
  std::size_t size() { return first_free - base; }

  std::size_t space() { return capacity; }
  void reserve(const std::size_t target_size);
  void check_size();
  void push_back(const ValueType &ele);
  void push_back(ValueType &&ele);
  void insert(const std::size_t index, const ValueType &ele);
  void insert(const Iterator it, const ValueType &ele);

  // constructors and copy controllers
  Vector();
  Vector(std::size_t init_size);
  Vector(std::size_t init_size, const ValueType &ele);
  Vector(const Vector &another);
  Vector(Vector &&another);
  ~Vector();

  // reload operators
  Vector &operator=(Vector another);
  ValueType &operator[](std::size_t index) { return *(base + index); }
};

// allocate space of target_size to the Vector,
// if the target_size is not bigger than the current capacity,
// do nothing
template <typename T> void Vector<T>::reserve(const std::size_t target_size) {
  if (target_size <= capacity) {
    return;
  }
  // allocate new space
  auto new_base = allo.allocate(target_size);

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
  capacity = target_size;
  after_end = base + capacity;
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
  std::swap(*this, another);
  return *this;
}

// to implement the class matrix, the type T should reload the following
// operators:
//+, -, *, /, +=, -=, *=, /=, -(the unary operator), ==. !=, <<, >>(input and
// output)
template <typename T> class Matrix {
private:
  using ValueType = T;
  std::size_t row_size, col_size, row_capacity;
};
} // namespace linear_algebra

#endif
