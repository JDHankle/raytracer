#ifndef _PLANE_H
#define _PLANE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Plane : public Object {
    Vect normal;
    double distance;
    Color color;

 public:

    Plane ();
    Plane (Vect, double, Color);

    Vect getPlaneNormal() { return normal; };
    double getPlaneDistance(){ return distance; };
    virtual Color getColor(){ return color; };

    Vect getNormalAt(Vect point) {
        return normal;
    }

    virtual double findIntersection(Ray ray) {
        Vect ray_direction = ray.getRayDirection();

        double a = ray_direction.dotProduct(normal);

        // ray does not intersect an object in the scene.
        if (a == 0) {
            return -1;
        }

        double b = normal.dotProduct(
            ray.getRayOrigin().vectAdd(
                normal.vectScale(distance).invert()
            )
        );

        return -1 * b / a;

    }
};

Plane::Plane() {
    normal = Vect(1, 0, 0);
    distance = 0;
    color = Color(0.5, 0.5, 0.5, 0);
}

Plane::Plane(Vect pnormal, double pdistance, Color pcolor) {
    normal = pnormal;
    distance = pdistance;
    color = pcolor;
}

#endif
