#ifndef FRACTION_H
#define FRACTION_H

typedef struct {
    int numerator;
    int denominator;
} Fraction;

/* Create and normalize a fraction; exits if denominator == 0 */
Fraction createFraction(int num, int den);

/* Fraction arithmetic */
Fraction fraction_add(Fraction a, Fraction b);
Fraction fraction_sub(Fraction a, Fraction b);
Fraction fraction_mul(Fraction a, Fraction b);
Fraction fraction_div(Fraction a, Fraction b);

/* Check if fraction a is divisible by b (i.e., a/b is an integer) */
int is_divisible_by(Fraction a, Fraction b);

/* Convert fraction to double */
double fraction_to_float(Fraction a);

/* Optional file I/O for Fractions */
void save_fraction_to_file(Fraction f, const char *filename);
Fraction* load_fractions_from_file(const char *filename, int *count);

#endif // FRACTION_H
