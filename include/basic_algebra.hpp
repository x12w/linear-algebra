#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>

#ifndef BASIC_ALGEBRA_HPP
#define BASIC_ALGEBRA_HPP

namespace basic_algebra {
class ComplexNumberType;
// 定义基本代数类型，用于定义矩阵元素，若后续需要修改元素类型，可直接替换
// the real number type should define the following operators:
// +, -, *, /, +=, -=, *=, /=, -(the unary operator), ==, !=
using RealNumber = double;

// the complex number type should define the following operators:
// +, -, *, /, +=, -=, *=, /=, -(the unary operator), ==, !=
using ComplexNumber = ComplexNumberType;

class ComplexNumberType {
private:
  RealNumber real_part, imaginary_part;

public:
  // constructors
  ComplexNumberType() : real_part(0), imaginary_part(0) {}
  ComplexNumberType(RealNumber _real_part, RealNumber _imaginary_part)
      : real_part(_real_part), imaginary_part(_imaginary_part) {}

  // reload operators

  // the compute operators
  ComplexNumberType operator+(const RealNumber &operand) const;
  ComplexNumberType &operator+=(const RealNumber &operand);
  ComplexNumberType operator+(const ComplexNumberType &operand) const;
  ComplexNumberType &operator+=(const ComplexNumberType &operand);
  ComplexNumberType operator-(const RealNumber &operand) const;
  ComplexNumberType &operator-=(const RealNumber &operand);
  ComplexNumberType operator-(const ComplexNumberType &operand) const;
  ComplexNumberType &operator-=(const ComplexNumberType &operand);
  ComplexNumberType operator*(const RealNumber &operand) const;
  ComplexNumberType &operator*=(const RealNumber &operand);
  ComplexNumberType operator*(const ComplexNumberType &operand) const;
  ComplexNumberType &operator*=(const ComplexNumberType &operand);
  ComplexNumberType operator/(const RealNumber &operand) const;
  ComplexNumberType &operator/=(const RealNumber &operand);
  ComplexNumberType operator/(const ComplexNumberType &operand) const;
  ComplexNumberType &operator/=(const ComplexNumberType &operand);
  ComplexNumberType operator-() const;
  bool operator==(const RealNumber &operand) const;
  bool operator==(const ComplexNumberType &operand) const;
  bool operator!=(const RealNumber &operand) const;
  bool operator!=(const ComplexNumberType &operand) const;

  // friends
  friend ComplexNumberType operator+(const RealNumber &operand1,
                                     const ComplexNumberType &operand2);
  friend ComplexNumberType operator-(const RealNumber &operand1,
                                     const ComplexNumberType &operand2);
  friend ComplexNumberType operator*(const RealNumber &operand1,
                                     const ComplexNumberType &operand2);
  friend ComplexNumberType operator/(const RealNumber &operand1,
                                     const ComplexNumberType &operand2);
  friend bool operator==(const RealNumber &operand1,
                         const ComplexNumberType &operand2);
  friend bool operator!=(const RealNumber &operand1,
                         const ComplexNumberType &operand2);

  friend std::ostream &operator<<(std::ostream &os,
                                  const ComplexNumberType &operand);

  friend std::istream &operator>>(std::istream &is, ComplexNumberType &operand);
};

ComplexNumberType operator+(const RealNumber &operand1,
                            const ComplexNumberType &operand2);
ComplexNumberType &operator+=(RealNumber &operand1,
                              const ComplexNumberType &operand2);
ComplexNumberType operator-(const RealNumber &operand1,
                            const ComplexNumberType &operand2);
ComplexNumberType &operator-=(RealNumber &operand1,
                              const ComplexNumberType &operand2);
ComplexNumberType operator*(const RealNumber &operand1,
                            const ComplexNumberType &operand2);
ComplexNumberType &operator*=(RealNumber &operand1,
                              const ComplexNumberType &operand2);
ComplexNumberType operator/(const RealNumber &operand1,
                            const ComplexNumberType &operand2);
ComplexNumberType &operator/=(RealNumber &operand1,
                              const ComplexNumberType &operand2);
bool operator==(const RealNumber &operand1, const ComplexNumberType &operand2);
bool operator!=(const RealNumber &operand1, const ComplexNumberType &operand2);

std::ostream &operator<<(std::ostream &os, const ComplexNumberType &operand);
std::istream &operator>>(std::istream &is, const ComplexNumberType &operand);

} // namespace basic_algebra

#endif
