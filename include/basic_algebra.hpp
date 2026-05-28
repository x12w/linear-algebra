#ifndef BASIC_ALGEBRA_HPP
#define BASIC_ALGEBRA_HPP

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace basic_algebra {

template <typename T> class ComplexNumber {
private:
  T real_part, imaginary_part;

public:
  ComplexNumber() : real_part(0), imaginary_part(0) {}
  ComplexNumber(T real, T imaginary = 0)
      : real_part(real), imaginary_part(imaginary) {}

  ComplexNumber operator+(const T &operand) const {
    return ComplexNumber(real_part + operand, imaginary_part);
  }
  ComplexNumber operator+(const ComplexNumber &operand) const {
    return ComplexNumber(real_part + operand.real_part,
                         imaginary_part + operand.imaginary_part);
  }
  ComplexNumber &operator+=(const T &operand) {
    real_part += operand;
    return *this;
  }
  ComplexNumber &operator+=(const ComplexNumber &operand) {
    real_part += operand.real_part;
    imaginary_part += operand.imaginary_part;
    return *this;
  }
  ComplexNumber operator-(const T &operand) const {
    return ComplexNumber(real_part - operand, imaginary_part);
  }
  ComplexNumber operator-(const ComplexNumber &operand) const {
    return ComplexNumber(real_part - operand.real_part,
                         imaginary_part - operand.imaginary_part);
  }
  ComplexNumber &operator-=(const T &operand) {
    real_part -= operand;
    return *this;
  }
  ComplexNumber &operator-=(const ComplexNumber &operand) {
    real_part -= operand.real_part;
    imaginary_part -= operand.imaginary_part;
    return *this;
  }
  ComplexNumber operator-() const {
    return ComplexNumber(-real_part, -imaginary_part);
  }
  ComplexNumber operator*(const T &operand) const {
    return ComplexNumber(real_part * operand, imaginary_part * operand);
  }
  ComplexNumber operator*(const ComplexNumber &operand) const {
    return ComplexNumber(
        real_part * operand.real_part - imaginary_part * operand.imaginary_part,
        real_part * operand.imaginary_part + imaginary_part * operand.real_part);
  }
  ComplexNumber &operator*=(const T &operand) {
    real_part *= operand;
    imaginary_part *= operand;
    return *this;
  }
  ComplexNumber &operator*=(const ComplexNumber &operand) {
    *this = *this * operand;
    return *this;
  }
  ComplexNumber operator/(const T &operand) const {
    return ComplexNumber(real_part / operand, imaginary_part / operand);
  }
  ComplexNumber operator/(const ComplexNumber &operand) const {
    T div = operand.real_part * operand.real_part +
            operand.imaginary_part * operand.imaginary_part;
    return ComplexNumber((real_part * operand.real_part +
                          imaginary_part * operand.imaginary_part) /
                             div,
                         (imaginary_part * operand.real_part -
                          real_part * operand.imaginary_part) /
                             div);
  }
  ComplexNumber &operator/=(const T &operand) {
    real_part /= operand;
    imaginary_part /= operand;
    return *this;
  }
  ComplexNumber &operator/=(const ComplexNumber &operand) {
    *this = *this / operand;
    return *this;
  }
  bool operator==(const T &operand) const {
    return real_part == operand && imaginary_part == 0;
  }
  bool operator==(const ComplexNumber &operand) const {
    return real_part == operand.real_part &&
           imaginary_part == operand.imaginary_part;
  }
  bool operator!=(const T &operand) const { return !(*this == operand); }
  bool operator!=(const ComplexNumber &operand) const {
    return !(*this == operand);
  }

  template <typename U>
  friend ComplexNumber<U> operator+(const U &lhs,
                                    const ComplexNumber<U> &rhs);
  template <typename U>
  friend ComplexNumber<U> operator-(const U &lhs,
                                    const ComplexNumber<U> &rhs);
  template <typename U>
  friend ComplexNumber<U> operator*(const U &lhs,
                                    const ComplexNumber<U> &rhs);
  template <typename U>
  friend ComplexNumber<U> operator/(const U &lhs,
                                    const ComplexNumber<U> &rhs);
  template <typename U>
  friend std::ostream &operator<<(std::ostream &os,
                                  const ComplexNumber<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is,
                                  ComplexNumber<U> &operand);
};

template <typename T>
ComplexNumber<T> operator+(const T &lhs, const ComplexNumber<T> &rhs) {
  return rhs + lhs;
}
template <typename T>
ComplexNumber<T> operator-(const T &lhs, const ComplexNumber<T> &rhs) {
  return ComplexNumber<T>(lhs - rhs.real_part, -rhs.imaginary_part);
}
template <typename T>
ComplexNumber<T> operator*(const T &lhs, const ComplexNumber<T> &rhs) {
  return rhs * lhs;
}
template <typename T>
ComplexNumber<T> operator/(const T &lhs, const ComplexNumber<T> &rhs) {
  return ComplexNumber<T>(lhs) / rhs;
}
template <typename T>
std::ostream &operator<<(std::ostream &os, const ComplexNumber<T> &operand) {
  if (operand.imaginary_part == 0) {
    return os << operand.real_part;
  }
  return os << operand.real_part << " + " << operand.imaginary_part << " i";
}
template <typename T>
std::istream &operator>>(std::istream &is, ComplexNumber<T> &operand) {
  return is >> operand.real_part >> operand.imaginary_part;
}

template <typename T = int> class RationalNumber {
private:
  T molecular, denominator;

  static T abs_value(T value) { return value < 0 ? -value : value; }
  static T gcd(T lhs, T rhs) {
    lhs = abs_value(lhs);
    rhs = abs_value(rhs);
    while (rhs != 0) {
      T temp = lhs % rhs;
      lhs = rhs;
      rhs = temp;
    }
    return lhs == 0 ? 1 : lhs;
  }
  void reduction() {
    if (denominator == 0) {
      throw(std::runtime_error("error: denominator cannot be 0"));
    }
    if (molecular == 0) {
      denominator = 1;
      return;
    }
    T div = gcd(molecular, denominator);
    molecular /= div;
    denominator /= div;
    if (denominator < 0) {
      molecular = -molecular;
      denominator = -denominator;
    }
  }

public:
  RationalNumber() : molecular(0), denominator(1) {}
  RationalNumber(const T &molecular_, const T &denominator_ = 1)
      : molecular(molecular_), denominator(denominator_) {
    reduction();
  }

  explicit operator long double() const {
    return static_cast<long double>(molecular) /
           static_cast<long double>(denominator);
  }

  RationalNumber operator-() const {
    return RationalNumber(-molecular, denominator);
  }
  RationalNumber operator+(const RationalNumber &operand) const {
    return RationalNumber(molecular * operand.denominator +
                              denominator * operand.molecular,
                          denominator * operand.denominator);
  }
  RationalNumber operator-(const RationalNumber &operand) const {
    return RationalNumber(molecular * operand.denominator -
                              denominator * operand.molecular,
                          denominator * operand.denominator);
  }
  RationalNumber operator*(const RationalNumber &operand) const {
    return RationalNumber(molecular * operand.molecular,
                          denominator * operand.denominator);
  }
  RationalNumber operator/(const RationalNumber &operand) const {
    if (operand.molecular == 0) {
      throw(std::runtime_error("error: RationalNumber divisor cannot be 0"));
    }
    return RationalNumber(molecular * operand.denominator,
                          denominator * operand.molecular);
  }
  RationalNumber &operator+=(const RationalNumber &operand) {
    *this = *this + operand;
    return *this;
  }
  RationalNumber &operator-=(const RationalNumber &operand) {
    *this = *this - operand;
    return *this;
  }
  RationalNumber &operator*=(const RationalNumber &operand) {
    *this = *this * operand;
    return *this;
  }
  RationalNumber &operator/=(const RationalNumber &operand) {
    *this = *this / operand;
    return *this;
  }
  bool operator==(const RationalNumber &operand) const {
    return molecular == operand.molecular && denominator == operand.denominator;
  }
  bool operator!=(const RationalNumber &operand) const {
    return !(*this == operand);
  }
  bool operator<(const RationalNumber &operand) const {
    return molecular * operand.denominator <
           denominator * operand.molecular;
  }
  bool operator<=(const RationalNumber &operand) const {
    return *this < operand || *this == operand;
  }
  bool operator>(const RationalNumber &operand) const {
    return operand < *this;
  }
  bool operator>=(const RationalNumber &operand) const {
    return operand <= *this;
  }

  template <typename U>
  friend std::ostream &operator<<(std::ostream &os,
                                  const RationalNumber<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is, RationalNumber<U> &operand);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const RationalNumber<T> &operand) {
  if (operand.denominator == 1) {
    return os << operand.molecular;
  }
  return os << operand.molecular << "/" << operand.denominator;
}
template <typename T>
std::istream &operator>>(std::istream &is, RationalNumber<T> &operand) {
  T molecular, denominator = 1;
  is >> molecular;
  if (is.peek() == '/') {
    is.get();
    is >> denominator;
  } else if (std::isspace(is.peek())) {
    std::streampos pos = is.tellg();
    T maybe_denominator;
    if (is >> maybe_denominator) {
      denominator = maybe_denominator;
    } else {
      is.clear();
      is.seekg(pos);
    }
  }
  operand = RationalNumber<T>(molecular, denominator);
  return is;
}

template <typename T = unsigned int> class BigInteger {
private:
  static constexpr int base = 1000000000;
  static constexpr int base_digits = 9;
  bool is_negative;
  std::vector<int> digits;

  void normalize() {
    while (!digits.empty() && digits.back() == 0) {
      digits.pop_back();
    }
    if (digits.empty()) {
      is_negative = false;
    }
  }
  static int abs_compare(const BigInteger &lhs, const BigInteger &rhs) {
    if (lhs.digits.size() != rhs.digits.size()) {
      return lhs.digits.size() < rhs.digits.size() ? -1 : 1;
    }
    for (std::size_t index = lhs.digits.size(); index-- > 0;) {
      if (lhs.digits[index] != rhs.digits[index]) {
        return lhs.digits[index] < rhs.digits[index] ? -1 : 1;
      }
    }
    return 0;
  }
  static BigInteger abs_add(const BigInteger &lhs, const BigInteger &rhs) {
    BigInteger result;
    long long carry = 0;
    std::size_t n = std::max(lhs.digits.size(), rhs.digits.size());
    for (std::size_t index = 0; index < n || carry != 0; ++index) {
      long long current = carry;
      if (index < lhs.digits.size()) {
        current += lhs.digits[index];
      }
      if (index < rhs.digits.size()) {
        current += rhs.digits[index];
      }
      result.digits.push_back(static_cast<int>(current % base));
      carry = current / base;
    }
    return result;
  }
  static BigInteger abs_sub(const BigInteger &lhs, const BigInteger &rhs) {
    BigInteger result;
    long long borrow = 0;
    for (std::size_t index = 0; index < lhs.digits.size(); ++index) {
      long long current = lhs.digits[index] - borrow;
      if (index < rhs.digits.size()) {
        current -= rhs.digits[index];
      }
      if (current < 0) {
        current += base;
        borrow = 1;
      } else {
        borrow = 0;
      }
      result.digits.push_back(static_cast<int>(current));
    }
    result.normalize();
    return result;
  }

public:
  BigInteger() : is_negative(false), digits() {}
  BigInteger(long long value) : is_negative(value < 0), digits() {
    unsigned long long x =
        value < 0 ? static_cast<unsigned long long>(-(value + 1)) + 1
                  : static_cast<unsigned long long>(value);
    while (x != 0) {
      digits.push_back(static_cast<int>(x % base));
      x /= base;
    }
    normalize();
  }
  BigInteger(const std::string &value) : is_negative(false), digits() {
    std::size_t pos = 0;
    if (pos < value.size() && (value[pos] == '-' || value[pos] == '+')) {
      is_negative = value[pos] == '-';
      ++pos;
    }
    for (std::size_t end = value.size(); end > pos;) {
      std::size_t start = end > static_cast<std::size_t>(base_digits)
                              ? end - base_digits
                              : pos;
      if (start < pos) {
        start = pos;
      }
      digits.push_back(std::stoi(value.substr(start, end - start)));
      end = start;
    }
    normalize();
  }

  explicit operator long double() const {
    long double result = 0;
    for (std::size_t index = digits.size(); index-- > 0;) {
      result = result * base + digits[index];
    }
    return is_negative ? -result : result;
  }
  BigInteger operator-() const {
    BigInteger result(*this);
    if (!result.digits.empty()) {
      result.is_negative = !result.is_negative;
    }
    return result;
  }
  BigInteger operator+(const BigInteger &operand) const {
    BigInteger result;
    if (is_negative == operand.is_negative) {
      result = abs_add(*this, operand);
      result.is_negative = is_negative;
    } else if (abs_compare(*this, operand) >= 0) {
      result = abs_sub(*this, operand);
      result.is_negative = is_negative;
    } else {
      result = abs_sub(operand, *this);
      result.is_negative = operand.is_negative;
    }
    result.normalize();
    return result;
  }
  BigInteger operator-(const BigInteger &operand) const {
    return *this + (-operand);
  }
  BigInteger operator*(const BigInteger &operand) const {
    BigInteger result;
    result.is_negative = is_negative != operand.is_negative;
    result.digits.assign(digits.size() + operand.digits.size(), 0);
    for (std::size_t i = 0; i < digits.size(); ++i) {
      long long carry = 0;
      for (std::size_t j = 0; j < operand.digits.size() || carry != 0; ++j) {
        long long current = result.digits[i + j] + carry;
        if (j < operand.digits.size()) {
          current += 1LL * digits[i] * operand.digits[j];
        }
        result.digits[i + j] = static_cast<int>(current % base);
        carry = current / base;
      }
    }
    result.normalize();
    return result;
  }
  BigInteger operator/(const BigInteger &operand) const {
    if (operand == 0) {
      throw(std::runtime_error("error: BigInteger divisor cannot be 0"));
    }
    BigInteger dividend(*this), divisor(operand), quotient, current;
    dividend.is_negative = false;
    divisor.is_negative = false;
    quotient.digits.assign(dividend.digits.size(), 0);
    for (std::size_t index = dividend.digits.size(); index-- > 0;) {
      current.digits.insert(current.digits.begin(), dividend.digits[index]);
      current.normalize();
      int low = 0, high = base - 1, best = 0;
      while (low <= high) {
        int mid = low + (high - low) / 2;
        BigInteger candidate = divisor * BigInteger(mid);
        if (candidate <= current) {
          best = mid;
          low = mid + 1;
        } else {
          high = mid - 1;
        }
      }
      quotient.digits[index] = best;
      current -= divisor * BigInteger(best);
    }
    quotient.is_negative = is_negative != operand.is_negative;
    quotient.normalize();
    return quotient;
  }
  BigInteger operator%(const BigInteger &operand) const {
    return *this - (*this / operand) * operand;
  }
  BigInteger &operator+=(const BigInteger &operand) {
    *this = *this + operand;
    return *this;
  }
  BigInteger &operator-=(const BigInteger &operand) {
    *this = *this - operand;
    return *this;
  }
  BigInteger &operator*=(const BigInteger &operand) {
    *this = *this * operand;
    return *this;
  }
  BigInteger &operator/=(const BigInteger &operand) {
    *this = *this / operand;
    return *this;
  }
  BigInteger &operator%=(const BigInteger &operand) {
    *this = *this % operand;
    return *this;
  }
  bool operator==(const BigInteger &operand) const {
    return is_negative == operand.is_negative && digits == operand.digits;
  }
  bool operator!=(const BigInteger &operand) const { return !(*this == operand); }
  bool operator<(const BigInteger &operand) const {
    if (is_negative != operand.is_negative) {
      return is_negative;
    }
    int cmp = abs_compare(*this, operand);
    return is_negative ? cmp > 0 : cmp < 0;
  }
  bool operator<=(const BigInteger &operand) const {
    return *this < operand || *this == operand;
  }
  bool operator>(const BigInteger &operand) const { return operand < *this; }
  bool operator>=(const BigInteger &operand) const { return operand <= *this; }

  template <typename U>
  friend std::ostream &operator<<(std::ostream &os,
                                  const BigInteger<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is, BigInteger<U> &operand);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const BigInteger<T> &operand) {
  if (operand.digits.empty()) {
    return os << 0;
  }
  if (operand.is_negative) {
    os << '-';
  }
  os << operand.digits.back();
  for (std::size_t index = operand.digits.size() - 1; index-- > 0;) {
    os << std::setw(BigInteger<T>::base_digits) << std::setfill('0')
       << operand.digits[index];
  }
  os << std::setfill(' ');
  return os;
}
template <typename T>
std::istream &operator>>(std::istream &is, BigInteger<T> &operand) {
  std::string value;
  is >> value;
  operand = BigInteger<T>(value);
  return is;
}

template <typename T = int> class HighPrecisionNumber {
private:
  BigInteger<> scaled_value;
  std::size_t scale;

  static BigInteger<> pow10(std::size_t n) {
    BigInteger<> result(1);
    for (std::size_t i = 0; i < n; ++i) {
      result *= BigInteger<>(10);
    }
    return result;
  }
  void align_scale(HighPrecisionNumber &other) {
    if (scale < other.scale) {
      scaled_value *= pow10(other.scale - scale);
      scale = other.scale;
    } else if (other.scale < scale) {
      other.scaled_value *= pow10(scale - other.scale);
      other.scale = scale;
    }
  }

public:
  HighPrecisionNumber() : scaled_value(0), scale(0) {}
  HighPrecisionNumber(long long value) : scaled_value(value), scale(0) {}
  HighPrecisionNumber(float value)
      : HighPrecisionNumber(static_cast<double>(value)) {}
  HighPrecisionNumber(double value) : scaled_value(0), scale(0) {
    std::ostringstream oss;
    oss << std::setprecision(std::numeric_limits<double>::max_digits10)
        << value;
    *this = HighPrecisionNumber(oss.str());
  }
  HighPrecisionNumber(const std::string &value) : scaled_value(0), scale(0) {
    std::string digits;
    bool after_dot = false;
    for (char ch : value) {
      if (ch == '.') {
        after_dot = true;
        continue;
      }
      if (after_dot && std::isdigit(static_cast<unsigned char>(ch))) {
        ++scale;
      }
      digits.push_back(ch);
    }
    scaled_value = BigInteger<>(digits.empty() ? "0" : digits);
  }
  explicit operator long double() const {
    return static_cast<long double>(scaled_value) /
           static_cast<long double>(pow10(scale));
  }
  HighPrecisionNumber operator-() const {
    HighPrecisionNumber result(*this);
    result.scaled_value = -result.scaled_value;
    return result;
  }
  HighPrecisionNumber operator+(HighPrecisionNumber operand) const {
    HighPrecisionNumber result(*this);
    result.align_scale(operand);
    result.scaled_value += operand.scaled_value;
    return result;
  }
  HighPrecisionNumber operator-(HighPrecisionNumber operand) const {
    HighPrecisionNumber result(*this);
    result.align_scale(operand);
    result.scaled_value -= operand.scaled_value;
    return result;
  }
  HighPrecisionNumber operator*(const HighPrecisionNumber &operand) const {
    HighPrecisionNumber result;
    result.scaled_value = scaled_value * operand.scaled_value;
    result.scale = scale + operand.scale;
    return result;
  }
  HighPrecisionNumber operator/(const HighPrecisionNumber &operand) const {
    if (operand.scaled_value == BigInteger<>(0)) {
      throw(std::runtime_error("error: HighPrecisionNumber divisor cannot be 0"));
    }
    HighPrecisionNumber result;
    const std::size_t extra_precision = 30;
    result.scaled_value =
        (scaled_value * pow10(operand.scale + extra_precision)) /
        (operand.scaled_value * pow10(scale));
    result.scale = extra_precision;
    return result;
  }
  HighPrecisionNumber &operator+=(const HighPrecisionNumber &operand) {
    *this = *this + operand;
    return *this;
  }
  HighPrecisionNumber &operator-=(const HighPrecisionNumber &operand) {
    *this = *this - operand;
    return *this;
  }
  HighPrecisionNumber &operator*=(const HighPrecisionNumber &operand) {
    *this = *this * operand;
    return *this;
  }
  HighPrecisionNumber &operator/=(const HighPrecisionNumber &operand) {
    *this = *this / operand;
    return *this;
  }
  bool operator==(HighPrecisionNumber operand) const {
    HighPrecisionNumber result(*this);
    result.align_scale(operand);
    return result.scaled_value == operand.scaled_value;
  }
  bool operator!=(const HighPrecisionNumber &operand) const {
    return !(*this == operand);
  }
  bool operator<(HighPrecisionNumber operand) const {
    HighPrecisionNumber result(*this);
    result.align_scale(operand);
    return result.scaled_value < operand.scaled_value;
  }
  bool operator<=(const HighPrecisionNumber &operand) const {
    return *this < operand || *this == operand;
  }
  bool operator>(const HighPrecisionNumber &operand) const {
    return operand < *this;
  }
  bool operator>=(const HighPrecisionNumber &operand) const {
    return operand <= *this;
  }

  template <typename U>
  friend std::ostream &operator<<(std::ostream &os,
                                  const HighPrecisionNumber<U> &operand);
  template <typename U>
  friend std::istream &operator>>(std::istream &is,
                                  HighPrecisionNumber<U> &operand);
};

template <typename T>
std::ostream &operator<<(std::ostream &os,
                         const HighPrecisionNumber<T> &operand) {
  std::ostringstream oss;
  oss << operand.scaled_value;
  std::string text = oss.str();
  bool negative = !text.empty() && text[0] == '-';
  if (negative) {
    text.erase(text.begin());
    os << '-';
  }
  if (operand.scale == 0) {
    return os << text;
  }
  if (text.size() <= operand.scale) {
    os << "0.";
    for (std::size_t i = text.size(); i < operand.scale; ++i) {
      os << '0';
    }
    return os << text;
  }
  return os << text.substr(0, text.size() - operand.scale) << "."
            << text.substr(text.size() - operand.scale);
}
template <typename T>
std::istream &operator>>(std::istream &is, HighPrecisionNumber<T> &operand) {
  std::string value;
  is >> value;
  operand = HighPrecisionNumber<T>(value);
  return is;
}

} // namespace basic_algebra

#endif
