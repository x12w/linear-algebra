#include "basic_algebra.hpp"
#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>

namespace basic_algebra {

// member functions
ComplexNumberType
ComplexNumberType::operator+(const RealNumber &operand) const {
  return ComplexNumberType(real_part + operand, imaginary_part);
}

ComplexNumberType
ComplexNumberType::operator+(const ComplexNumberType &operand) const {
  return ComplexNumberType(real_part + operand.real_part,
                           imaginary_part + operand.imaginary_part);
}

ComplexNumberType &ComplexNumberType::operator+=(const RealNumber &operand) {
  real_part += operand;
  return *this;
}

ComplexNumberType &
ComplexNumberType::operator+=(const ComplexNumberType &operand) {
  real_part += operand.real_part;
  imaginary_part += operand.imaginary_part;
  return *this;
}

ComplexNumberType
ComplexNumberType::operator-(const RealNumber &operand) const {
  return ComplexNumberType(real_part - operand, imaginary_part);
}

ComplexNumberType
ComplexNumberType::operator-(const ComplexNumberType &operand) const {
  return ComplexNumberType(real_part - operand.real_part,
                           imaginary_part - operand.imaginary_part);
}

ComplexNumberType &ComplexNumberType::operator-=(const RealNumber &operand) {
  real_part -= operand;
  return *this;
}

ComplexNumberType &
ComplexNumberType::operator-=(const ComplexNumberType &operand) {
  real_part -= operand.real_part;
  imaginary_part -= operand.imaginary_part;
  return *this;
}

ComplexNumberType ComplexNumberType::operator-() const {
  return ComplexNumberType(-real_part, -imaginary_part);
}

ComplexNumberType
ComplexNumberType::operator*(const RealNumber &operand) const {
  return ComplexNumberType(real_part * operand, imaginary_part * operand);
}

ComplexNumberType
ComplexNumberType::operator*(const ComplexNumberType &operand) const {
  return ComplexNumberType(
      real_part * operand.real_part - imaginary_part * operand.imaginary_part,
      real_part * operand.imaginary_part + imaginary_part * operand.real_part);
}

ComplexNumberType &ComplexNumberType::operator*=(const RealNumber &operand) {
  real_part *= operand;
  imaginary_part *= operand;
  return *this;
}

ComplexNumberType &
ComplexNumberType::operator*=(const ComplexNumberType &operand) {
  auto real_part_temp =
      real_part * operand.real_part - imaginary_part * operand.imaginary_part;
  imaginary_part =
      real_part * operand.imaginary_part + imaginary_part * operand.real_part;
  real_part = real_part_temp;
  return *this;
}

ComplexNumberType
ComplexNumberType::operator/(const RealNumber &operand) const {
  return ComplexNumberType(real_part / operand, imaginary_part / operand);
}

ComplexNumberType
ComplexNumberType::operator/(const ComplexNumberType &operand) const {
  auto div = operand.real_part * operand.real_part +
             operand.imaginary_part * operand.imaginary_part;
  return ComplexNumberType((real_part * operand.real_part +
                            imaginary_part * operand.imaginary_part) /
                               div,
                           (imaginary_part * operand.real_part -
                            real_part * operand.imaginary_part) /
                               div);
}

ComplexNumberType &ComplexNumberType::operator/=(const RealNumber &operand) {
  real_part /= operand;
  imaginary_part /= operand;
  return *this;
}

ComplexNumberType &
ComplexNumberType::operator/=(const ComplexNumberType &operand) {
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

bool ComplexNumberType::operator==(const RealNumber &operand) const {
  return (real_part == operand && imaginary_part == 0);
}

bool ComplexNumberType::operator==(const ComplexNumberType &operand) const {
  return (real_part == operand.real_part &&
          imaginary_part == operand.imaginary_part);
}

bool ComplexNumberType::operator!=(const RealNumber &operand) const {
  return (real_part != operand || imaginary_part != 0);
}

bool ComplexNumberType::operator!=(const ComplexNumberType &operand) const {
  return (real_part != operand.real_part ||
          imaginary_part != operand.imaginary_part);
}

// normal functions
ComplexNumberType operator+(const RealNumber &operand1,
                            const ComplexNumberType &operand2) {
  return operand2 + operand1;
}

ComplexNumberType operator-(const RealNumber &operand1,
                            const ComplexNumberType &operand2) {
  return ComplexNumberType(operand1 - operand2.real_part,
                           -operand2.imaginary_part);
  ;
}

ComplexNumberType operator*(const RealNumber &operand1,
                            const ComplexNumberType &operand2) {
  return operand2 * operand1;
}

ComplexNumberType operator/(const RealNumber &operand1,
                            const ComplexNumberType &operand2) {
  auto div = operand2.real_part * operand2.real_part +
             operand2.imaginary_part * operand2.imaginary_part;
  return ComplexNumberType(operand1 * operand2.real_part / div,
                           -operand1 * operand2.imaginary_part / div);
}

bool operator==(const RealNumber &operand1, const ComplexNumberType &operand2) {
  return operand2 == operand1;
}

bool operator!=(const RealNumber &operand1, const ComplexNumberType &operand2) {
  return operand2 != operand1;
}

std::ostream &operator<<(std::ostream &os, const ComplexNumberType &operand) {
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

std::istream &operator>>(std::istream &is, ComplexNumberType &operand) {
  is >> operand.real_part >> operand.imaginary_part;
  return is;
}

} // namespace basic_algebra
