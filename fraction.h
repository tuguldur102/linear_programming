#ifndef FRACTION_H
#define FRACTION_H

#include <iostream>

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

    // For comparing pivot == 0, etc.
    bool operator==(const Fraction &other) const;
    bool operator!=(const Fraction &other) const;

    // Convert fraction to double
    double toDouble() const;

    // Print fraction in reduced form, with denominator=1 shown as just integer, 0/1 => 0, etc.
    friend std::ostream &operator<<(std::ostream &os, const Fraction &fr);
};

#endif // FRACTION_H
