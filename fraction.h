#ifndef FRACTION_H
#define FRACTION_H

#include <iostream>
#include <string>

struct Fraction {
    int numerator;
    int denominator;

    Fraction(int num = 0, int den = 1);
    void normalize();

    // Overloaded operators
    Fraction operator+(const Fraction &other) const;
    Fraction operator-(const Fraction &other) const;
    Fraction operator*(const Fraction &other) const;
    Fraction operator/(const Fraction &other) const;

    bool operator==(const Fraction &other) const;
    bool operator!=(const Fraction &other) const;
    bool operator<(const Fraction &other) const;
    bool operator>(const Fraction &other) const;

    // Convert fraction to double
    double toDouble() const;

    friend std::ostream &operator<<(std::ostream &os, const Fraction &fr);
};

#endif
