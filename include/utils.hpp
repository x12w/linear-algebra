#include <cstddef>
#include <memory>
#include <stdexcept>

#ifndef UTILS_HPP
#define UTILS_HPP

namespace utils {

template <typename T> class Array {
private:
  using ValueType = T;
  using Iterator = ValueType *;
  using CIterator = const ValueType *;
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
  void swap(Array &another) {
    std::swap(capacity, another.capacity);
    std::swap(base, another.base);
    std::swap(first_free, another.first_free);
    std::swap(after_end, another.after_end);
  }
  Iterator begin() const { return base; }
  Iterator end() const { return first_free; }
  CIterator cbegin() const { return const_cast<CIterator>(base); }
  CIterator cend() const { return const_cast<CIterator>(base); }

  // constructors and copy controllers
  Array();
  Array(std::size_t init_size);
  Array(std::size_t init_size, const ValueType &ele);
  Array(const Array &another);
  Array(Array &&another);
  ~Array();

  // reload operators
  Array &operator=(Array another);
  ValueType &operator[](std::size_t index) { return base[index]; }
  const ValueType &operator[](std::size_t index) const { return base[index]; }
  Array operator*(const ValueType &operand) const;
  Array &operator*=(const ValueType &operand);
  Array operator+(const Array &operand) const;
  Array &operator+=(const Array &operand);
  Array operator-(const Array &operand) const;
  Array &operator-=(const Array &operand);
  Array operator-() const;

  // friends
  template <typename U>
  friend Array operator*(const ValueType &operand1, const Array &operand2);
  template <typename U>
  friend std::ostream &operator<<(std::ostream &os, const Array<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is, Array<U> &operand);
};

// allocate space of target_size to the Array,
// if the target_size is not bigger than the current capacity,
// do nothing
template <typename T>
void Array<T>::reserve(const std::size_t target_capacity) {
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
template <typename T> void Array<T>::resize(std::size_t target_size) {
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

template <typename T> void Array<T>::check_size() {
  if (first_free == after_end) {
    reserve(capacity == 0 ? 1 : 2 * capacity);
  }
}

template <typename T> void Array<T>::push_back(const ValueType &ele) {
  // check is there enough space
  check_size();

  allo.construct(first_free, ele);
  first_free++;
}

template <typename T> void Array<T>::push_back(ValueType &&ele) {
  // check is there enough space
  check_size();

  allo.construct(first_free, std::move(ele));
  first_free++;
}

template <typename T> auto Array<T>::pop_back() -> ValueType {
  auto res = std::move(*(base + size() - 1));
  allo.destroy(base + size() - 1);
  first_free--;
  return res;
}

// the insert method assume that the index/iterator is valid :)
template <typename T>
void Array<T>::insert(const std::size_t index, const ValueType &ele) {
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
void Array<T>::insert(const Iterator it, const ValueType &ele) {
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

template <typename T> void Array<T>::clear() {
  for (; first_free > base; first_free--) {
    allo.destroy(first_free - 1);
  }
}

template <typename T> Array<T>::Array() {
  base = allo.allocate(0);
  first_free = base;
  after_end = base;
  capacity = 0;
}

template <typename T> Array<T>::Array(std::size_t init_size) {
  base = allo.allocate(init_size);
  first_free = base + init_size;
  capacity = init_size;
  after_end = base + capacity;
  for (auto i = base; i < first_free; i++) {
    allo.construct(i);
  }
}

template <typename T>
Array<T>::Array(std::size_t init_size, const ValueType &ele) {
  base = allo.allocate(init_size);
  first_free = base + init_size;
  capacity = init_size;
  after_end = base + capacity;
  for (auto i = base; i < first_free; i++) {
    allo.construct(i, ele);
  }
}

template <typename T> Array<T>::Array(const Array &another) {
  base = allo.allocate(another.size());
  first_free = base;
  capacity = another.size();
  after_end = base + capacity;

  for (auto i = another.base; i < another.first_free; i++, first_free++) {
    allo.construct(first_free, *i);
  }
}

template <typename T> Array<T>::Array(Array &&another) {
  base = allo.allocate(another.size());
  first_free = base;
  capacity = another.size();
  after_end = base + capacity;

  for (auto i = another.base; i < another.first_free; i++, first_free++) {
    allo.construct(first_free, std::move_if_noexcept(*i));
  }
}

template <typename T> Array<T>::~Array() {
  for (auto i = base; i < first_free; i++) {
    allo.destroy(i);
  }
  allo.deallocate(base, capacity);
}

template <typename T> Array<T> &Array<T>::operator=(Array another) {
  swap(another);
  return *this;
}

template <typename T>
auto Array<T>::operator*(const ValueType &operand) const -> Array {
  Array res(*this);

  for (std::size_t index = 0; index < res.size(); index++) {
    res[index] *= operand;
  }

  return res;
}

template <typename T>
auto Array<T>::operator*=(const ValueType &operand) -> Array & {
  for (std::size_t index = 0; index < size(); index++) {
    (*this)[index] *= operand;
  }

  return *this;
}

template <typename T>
auto Array<T>::operator+(const Array &operand) const -> Array {
  // check the Arrays' size
  if (size() != operand.size()) {
    throw(std::runtime_error("error: to do a Array add, you should make sure "
                             "that the two Arrays' size equal"));
  }

  Array res(*this);

  for (std::size_t index = 0; index < res.size(); index++) {
    res[index] += operand[index];
  }

  return res;
}

template <typename T>
auto Array<T>::operator+=(const Array &operand) -> Array & {
  *this = (*this) + operand;
  return *this;
}

template <typename T>
auto Array<T>::operator-(const Array &operand) const -> Array {
  // check the Arrays' size
  if (size() != operand.size()) {
    throw(std::runtime_error("error: to do a Array subtraction, you should "
                             "make sure that the two Arrays' size equal"));
  }

  Array res(*this);

  for (std::size_t index = 0; index < res.size(); index++) {
    res[index] -= operand[index];
  }

  return res;
}

template <typename T>
auto Array<T>::operator-=(const Array &operand) -> Array & {
  *this = (*this) - operand;
  return *this;
}

template <typename T> auto Array<T>::operator-() const -> Array {
  Array res(size());
  for (std::size_t index = 0; index < size(); index++) {
    res[index] = -(*this)[index];
  }
  return res;
}

// friends

template <typename T>
std::ostream &operator<<(std::ostream &os, const Array<T> &operand) {
  for (std::size_t index = 0; index < operand.size(); index++) {
    os << operand[index] << " ";
  }

  return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, Array<T> &operand) {
  for (std::size_t index = 0; index < operand.size(); index++) {
    is >> operand[index];
  }
  return is;
}

} // namespace utils

#endif
