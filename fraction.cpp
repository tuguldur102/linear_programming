#include "fraction.h"
#include <stdexcept>
#include <algorithm>
#include <cstdlib>
using namespace std;

static int gcd(int a, int b) 
{
    a = (a < 0) ? -a : a;
    b = (b < 0) ? -b : b;
    return (b == 0) ? a : gcd(b, a % b);
}

Fraction::Fraction(int num, int den) : numerator(num), denominator(den) 
{
    if (den == 0) 
    {
        throw runtime_error("Fraction: Denominator cannot be zero.");
    }
    normalize();
}

void Fraction::normalize() 
{
    int g = gcd(numerator, denominator);
    numerator   /= g;
    denominator /= g;
    if (denominator < 0) 
    {
        denominator = -denominator;
        numerator   = -numerator;
    }
}

// Arithmetic operators
Fraction Fraction::operator+(const Fraction &other) const 
{
    // a/b + c/d = (a*d + b*c)/(b*d)
    int num = numerator * other.denominator + denominator * other.numerator;
    int den = denominator * other.denominator;
    return Fraction(num, den);
}

Fraction Fraction::operator-(const Fraction &other) const 
{
    // a/b - c/d = (a*d - b*c)/(b*d)
    int num = numerator * other.denominator - denominator * other.numerator;
    int den = denominator * other.denominator;
    return Fraction(num, den);
}

Fraction Fraction::operator*(const Fraction &other) const 
{
    // (a/b)*(c/d) = (a*c)/(b*d)
    int num = numerator * other.numerator;
    int den = denominator * other.denominator;
    return Fraction(num, den);
}

Fraction Fraction::operator/(const Fraction &other) const 
{
    // (a/b)/(c/d) = (a*d)/(b*c)
    if (other.numerator == 0) {
        throw runtime_error("Fraction: Division by zero fraction.");
    }
    int num = numerator * other.denominator;
    int den = denominator * other.numerator;
    return Fraction(num, den);
}

// Comparison
bool Fraction::operator==(const Fraction &other) const 
{
    return (numerator == other.numerator && denominator == other.denominator);
}
bool Fraction::operator!=(const Fraction &other) const
{
    return !(*this == other);
}

double Fraction::toDouble() const 
{
    return static_cast<double>(numerator) / static_cast<double>(denominator);
}

// Custom printing: if denominator == 1 => just print numerator; if numerator == 0 => 0
ostream &operator<<(ostream &os, const Fraction &fr) 
{
    if (fr.numerator == 0) {
        os << 0;
    } else if (fr.denominator == 1) {
        os << fr.numerator;
    } else {
        os << fr.numerator << "/" << fr.denominator;
    }
    return os;
}
