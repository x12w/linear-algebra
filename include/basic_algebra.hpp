#include <cmath>
#include <csignal>
#include <iostream>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <stdexcept>

#ifndef BASIC_ALGEBRA_HPP
#define BASIC_ALGEBRA_HPP

namespace basic_algebra {
// the real number type should define the following operators:
// +, -, *, /, +=, -=, *=, /=, -(the unary operator), ==, !=
// the complex number type should define the following operators:
// +, -, *, /, +=, -=, *=, /=, -(the unary operator), ==, !=

template <typename T> class ComplexNumber {
private:
  T real_part, imaginary_part;

public:
  // constructors
  ComplexNumber() : real_part(0), imaginary_part(0) {}
  ComplexNumber(T _real_part, T _imaginary_part)
      : real_part(_real_part), imaginary_part(_imaginary_part) {}

  // reload operators

  // the compute operators
  ComplexNumber operator+(const T &operand) const;
  ComplexNumber &operator+=(const T &operand);
  ComplexNumber operator+(const ComplexNumber &operand) const;
  ComplexNumber &operator+=(const ComplexNumber &operand);
  ComplexNumber operator-(const T &operand) const;
  ComplexNumber &operator-=(const T &operand);
  ComplexNumber operator-(const ComplexNumber &operand) const;
  ComplexNumber &operator-=(const ComplexNumber &operand);
  ComplexNumber operator*(const T &operand) const;
  ComplexNumber &operator*=(const T &operand);
  ComplexNumber operator*(const ComplexNumber &operand) const;
  ComplexNumber &operator*=(const ComplexNumber &operand);
  ComplexNumber operator/(const T &operand) const;
  ComplexNumber &operator/=(const T &operand);
  ComplexNumber operator/(const ComplexNumber &operand) const;
  ComplexNumber &operator/=(const ComplexNumber &operand);
  ComplexNumber operator-() const;
  bool operator==(const T &operand) const;
  bool operator==(const ComplexNumber &operand) const;
  bool operator!=(const T &operand) const;
  bool operator!=(const ComplexNumber &operand) const;

  // friends
  template <typename U>
  friend ComplexNumber<U> operator+(const U &operand1,
                                    const ComplexNumber<U> &operand2);
  template <typename U>
  friend ComplexNumber<U> operator-(const U &operand1,
                                    const ComplexNumber<U> &operand2);
  template <typename U>
  friend ComplexNumber<U> operator*(const U &operand1,
                                    const ComplexNumber<U> &operand2);
  template <typename U>
  friend ComplexNumber<U> operator/(const U &operand1,
                                    const ComplexNumber<U> &operand2);
  template <typename U>
  friend bool operator==(const U &operand1, const ComplexNumber<U> &operand2);
  template <typename U>
  friend bool operator!=(const U &operand1, const ComplexNumber<U> &operand2);

  template <typename U>
  friend std::ostream &operator<<(std::ostream &os,
                                  const ComplexNumber<U> &operand);

  template <typename U>
  friend std::istream &operator>>(std::istream &is, ComplexNumber<U> &operand);
};

template <typename T>
ComplexNumber<T> operator+(const T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> &operator+=(T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> operator-(const T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> &operator-=(T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> operator*(const T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> &operator*=(T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> operator/(const T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
ComplexNumber<T> &operator/=(T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
bool operator==(const T &operand1, const ComplexNumber<T> &operand2);
template <typename T>
bool operator!=(const T &operand1, const ComplexNumber<T> &operand2);

template <typename T>
std::ostream &operator<<(std::ostream &os, const ComplexNumber<T> &operand);
template <typename T>
std::istream &operator>>(std::istream &is, const ComplexNumber<T> &operand);

// the definitions

// member functions
template <typename T>
ComplexNumber<T> ComplexNumber<T>::operator+(const T &operand) const {
  return ComplexNumber(real_part + operand, imaginary_part);
}

template <typename T>
ComplexNumber<T>
ComplexNumber<T>::operator+(const ComplexNumber &operand) const {
  return ComplexNumber(real_part + operand.real_part,
                       imaginary_part + operand.imaginary_part);
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator+=(const T &operand) {
  real_part += operand;
  return *this;
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator+=(const ComplexNumber &operand) {
  real_part += operand.real_part;
  imaginary_part += operand.imaginary_part;
  return *this;
}

template <typename T>
ComplexNumber<T> ComplexNumber<T>::operator-(const T &operand) const {
  return ComplexNumber(real_part - operand, imaginary_part);
}

template <typename T>
ComplexNumber<T>
ComplexNumber<T>::operator-(const ComplexNumber &operand) const {
  return ComplexNumber(real_part - operand.real_part,
                       imaginary_part - operand.imaginary_part);
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator-=(const T &operand) {
  real_part -= operand;
  return *this;
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator-=(const ComplexNumber &operand) {
  real_part -= operand.real_part;
  imaginary_part -= operand.imaginary_part;
  return *this;
}

template <typename T> ComplexNumber<T> ComplexNumber<T>::operator-() const {
  return ComplexNumber(-real_part, -imaginary_part);
}

template <typename T>
ComplexNumber<T> ComplexNumber<T>::operator*(const T &operand) const {
  return ComplexNumber(real_part * operand, imaginary_part * operand);
}

template <typename T>
ComplexNumber<T>
ComplexNumber<T>::operator*(const ComplexNumber &operand) const {
  return ComplexNumber(
      real_part * operand.real_part - imaginary_part * operand.imaginary_part,
      real_part * operand.imaginary_part + imaginary_part * operand.real_part);
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator*=(const T &operand) {
  real_part *= operand;
  imaginary_part *= operand;
  return *this;
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator*=(const ComplexNumber &operand) {
  auto real_part_temp =
      real_part * operand.real_part - imaginary_part * operand.imaginary_part;
  imaginary_part =
      real_part * operand.imaginary_part + imaginary_part * operand.real_part;
  real_part = real_part_temp;
  return *this;
}

template <typename T>
ComplexNumber<T> ComplexNumber<T>::operator/(const T &operand) const {
  return ComplexNumber(real_part / operand, imaginary_part / operand);
}

template <typename T>
ComplexNumber<T>
ComplexNumber<T>::operator/(const ComplexNumber &operand) const {
  auto div = operand.real_part * operand.real_part +
             operand.imaginary_part * operand.imaginary_part;
  return ComplexNumber((real_part * operand.real_part +
                        imaginary_part * operand.imaginary_part) /
                           div,
                       (imaginary_part * operand.real_part -
                        real_part * operand.imaginary_part) /
                           div);
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator/=(const T &operand) {
  real_part /= operand;
  imaginary_part /= operand;
  return *this;
}

template <typename T>
ComplexNumber<T> &ComplexNumber<T>::operator/=(const ComplexNumber &operand) {
  auto div = operand.real_part * operand.real_part +
             operand.imaginary_part * operand.imaginary_part;
  auto real_part_temp = (real_part * operand.real_part +
                         imaginary_part * operand.imaginary_part) /
                        div;
  imaginary_part = (imaginary_part * operand.real_part -
                    real_part * operand.imaginary_part) /
                   div;
  real_part = real_part_temp;
  return *this;
}

template <typename T>
bool ComplexNumber<T>::operator==(const T &operand) const {
  return (real_part == operand && imaginary_part == 0);
}

template <typename T>
bool ComplexNumber<T>::operator==(const ComplexNumber &operand) const {
  return (real_part == operand.real_part &&
          imaginary_part == operand.imaginary_part);
}

template <typename T>
bool ComplexNumber<T>::operator!=(const T &operand) const {
  return (real_part != operand || imaginary_part != 0);
}

template <typename T>
bool ComplexNumber<T>::operator!=(const ComplexNumber &operand) const {
  return (real_part != operand.real_part ||
          imaginary_part != operand.imaginary_part);
}

// normal functions
template <typename T>
ComplexNumber<T> operator+(const T &operand1,
                           const ComplexNumber<T> &operand2) {
  return operand2 + operand1;
}

template <typename T>
ComplexNumber<T> operator-(const T &operand1,
                           const ComplexNumber<T> &operand2) {
  return ComplexNumber(operand1 - operand2.real_part, -operand2.imaginary_part);
  ;
}

template <typename T>
ComplexNumber<T> operator*(const T &operand1,
                           const ComplexNumber<T> &operand2) {
  return operand2 * operand1;
}
template <typename T>
ComplexNumber<T> operator/(const T &operand1,
                           const ComplexNumber<T> &operand2) {
  auto div = operand2.real_part * operand2.real_part +
             operand2.imaginary_part * operand2.imaginary_part;
  return ComplexNumber(operand1 * operand2.real_part / div,
                       -operand1 * operand2.imaginary_part / div);
}

template <typename T>
bool operator==(const T &operand1, const ComplexNumber<T> &operand2) {
  return operand2 == operand1;
}

template <typename T>
bool operator!=(const T &operand1, const ComplexNumber<T> &operand2) {
  return operand2 != operand1;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const ComplexNumber<T> &operand) {
  if (operand.real_part == 0 && operand.imaginary_part == 0) {
    os << 0;
    return os;
  }
  if (operand.real_part == 0) {
    os << operand.imaginary_part << " i";
    return os;
  }
  if (operand.imaginary_part == 0) {
    os << operand.real_part;
    return os;
  }
  os << operand.real_part << " + " << operand.imaginary_part << " i";
  return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, ComplexNumber<T> &operand) {
  is >> operand.real_part >> operand.imaginary_part;
  return is;
}

// to define the rational number type, you should provide a integer type,
// the integer type should imlement the following operators
// +, -, *, /, %, +=, -=, /=, *=, %=, -(the unary operator), ==, !=, >, >=, <,
// <=, <<, >>(stream operator) the default interger type is int
template <typename T = int> class RationalNumber {
private:
  using ValueType = T;
  ValueType molecular, denominator;

  // math methods to imlement the type
  ValueType gcd(ValueType operand1, ValueType operand2) const {
    operand1 = operand1 >= 0 ? operand1 : -operand1;
    operand2 = operand2 >= 0 ? operand2 : -operand2;

    while (operand2 != 0) {
      ValueType temp = operand1 % operand2;
      operand1 = operand2;
      operand2 = temp;
    }

    return operand1;
  }

  void reduction() {
    // check whether the molecular is 0
    if (molecular == 0) {
      denominator = 1;
      return;
    }

    ValueType div = gcd(molecular, denominator);
    molecular /= div;
    denominator /= div;
  }

public:
  // constructors
  RationalNumber() : molecular(0), denominator(1) {}
  RationalNumber(const ValueType &d, const ValueType &m = 1)
      : molecular(d), denominator(m) {
    // check whether the denominator is 0
    if (m == 0) {
      throw(std::runtime_error(
          "error: you can't create a RationalNumber whose denominator is 0"));
    }
  }

  // reload operators
  RationalNumber operator+(const RationalNumber &operand) const {
    ValueType new_molecular = molecular * operand.denominator +
                              denominator * operand.molecular,
              new_denominator = denominator * operand.denominator;
    RationalNumber res(new_molecular, new_denominator);
    res.reduction();
    return res;
  }

  RationalNumber operator-(const RationalNumber &operand) const {
    ValueType new_molecular = molecular * operand.denominator -
                              denominator * operand.molecular,
              new_denominator = denominator * operand.denominator;
    RationalNumber res(new_molecular, new_denominator);
    res.reduction();
    return res;
  }

  RationalNumber operator*(const RationalNumber &operand) const {
    RationalNumber res(molecular * operand.molecular,
                       denominator * operand.denominator);
    res.reduction();
    return res;
  }

  RationalNumber operator/(const RationalNumber &operand) const {
    RationalNumber res(molecular * operand.denominator,
                       denominator * operand.molecular);
    return res;
  }

  RationalNumber &operator+=(const RationalNumber &operand) {
    *this = (*this) + operand;
    return *this;
  }

  RationalNumber &operator-=(const RationalNumber &operand) {
    *this = (*this) - operand;
    return *this;
  }

  RationalNumber &operator*=(const RationalNumber &operand) {
    *this = (*this) * operand;
    return *this;
  }

  RationalNumber &operator/=(const RationalNumber &operand) {
    // check whether the operand is 0
    if (operand.molecular == 0) {
      throw(std::runtime_error("error: a RationalNumber 0 can't be a divisor"));
    }
    *this = (*this) + operand;
    return *this;
  }

  bool operator==(const RationalNumber &operand) const {
    return ((molecular * operand.denominator) ==
            (denominator * operand.molecular));
  }

  bool operator!=(const RationalNumber &operand) const {
    return ((molecular * operand.denominator) !=
            (denominator * operand.molecular));
  }

  bool operator>(const RationalNumber &operand) const {
    return ((molecular * operand.denominator) >
            (denominator * operand.molecular));
  }

  bool operator>=(const RationalNumber &operand) const {
    return ((molecular * operand.denominator) >=
            (denominator * operand.molecular));
  }

  bool operator<(const RationalNumber &operand) const {
    return ((molecular * operand.denominator) <
            (denominator * operand.molecular));
  }

  bool operator<=(const RationalNumber &operand) const {
    return ((molecular * operand.denominator) <=
            (denominator * operand.molecular));
  }

  template <typename U>
  friend std::ostream &operator<<(std::ostream &os,
                                  const RationalNumber<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is, RationalNumber<U> &operand);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const RationalNumber<T> &operand) {
  // check the special conditions
  if (operand.molecular == 0) {
    os << 0;
    return os;
  }
  if (operand.denominator == 1) {
    os << operand.molecular;
    return os;
  }
  if (operand.molecular < 0) {
    os << "-" << operand.molecular << "/" << operand.denominator;
    return os;
  }

  os << operand.molecular << "/" << operand.denominator;
  return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, RationalNumber<T> &operand) {
  is >> operand.molecular >> operand.denominator;
  return is;
}
} // namespace basic_algebra

#endif
