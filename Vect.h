#ifndef _VECT_H
#define _VECT_H

#include "math.h"

class Vect {
    double x, y, z;

 public:

    Vect ();
    Vect (double, double, double);

    double getVectX() { return x; }
    double getVectY() { return y; }
    double getVectZ() { return z; }

    double magnitude() {
        return sqrt( (x*x) + (y*y) + (z*z) );
    }

    double dotProduct(Vect other_vector) {
        return(
                 x * other_vector.getVectX()
               + y * other_vector.getVectY()
               + z * other_vector.getVectZ()
               );
    }

    Vect normalize() {
        double magnitude = sqrt( (x*x) + (y*y) + (z*z) );

        return Vect(x/magnitude, y/magnitude, z/magnitude);
    }

    Vect invert() {
        return Vect(-x, -y, -x);
    }

    Vect crossProduct(Vect other_vector) {
        return(
               Vect(
                    y * other_vector.getVectZ() - z * other_vector.getVectY(),
                    z * other_vector.getVectX() - x * other_vector.getVectZ(),
                    x * other_vector.getVectY() - y * other_vector.getVectX()
                    )
               );
    }

    Vect vectAdd(Vect other_vector) {
        return(
               Vect(
                    x + other_vector.getVectX(),
                    y + other_vector.getVectY(),
                    z + other_vector.getVectZ()
                    )
               );
    }

    Vect vectScale(double scalar) {
        return(
               Vect(
                    x * scalar,
                    y * scalar,
                    z * scalar
                    )
               );
    }

};

Vect::Vect() {
    x = 0;
    y = 0;
    z = 0;
}

Vect::Vect(double px, double py, double pz) {
    x = px;
    y = py;
    z = pz;
}

#endif
