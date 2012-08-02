
#ifndef SOBOL_PRIMITIVES_H
#define SOBOL_PRIMITIVES_H

#define max_m 17

/**
 * Each primitive is stored as a struct where
 *  dimension is the dimension number of the polynomial (unused)
 *  degree is the degree of the polynomial
 *  a is a binary word representing the coefficients 
 *  m is the array of m values
 */
struct primitive
{
    unsigned int dimension;
    unsigned int degree;
    unsigned int a;
    unsigned int m[max_m];
};

extern const struct primitive sobolPrimitives[];

#endif
