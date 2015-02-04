#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Sphere : public Object {
    Vect center;
    double radius;
    Color color;

 public:

    Sphere ();
    Sphere (Vect, double, Color);

    Vect getSphereCenter() { return center; };
    double getSphereRadius(){ return radius; };
    virtual Color getColor(){ return color; };

    Vect getNormalAt(Vect point) {
        // normal points away from the center of a sphere.
        Vect normal = point.vectAdd(center.invert()).normalize();
        return normal;
    }

    bool circle(){
        return true;
    }


    virtual double findIntersection(Ray ray) {
        Vect ray_origin = ray.getRayOrigin();
        double ray_origin_x = ray_origin.getVectX();
        double ray_origin_y = ray_origin.getVectY();
        double ray_origin_z = ray_origin.getVectZ();

        Vect ray_direction = ray.getRayDirection();
        double ray_direction_x = ray_direction.getVectX();
        double ray_direction_y = ray_direction.getVectY();
        double ray_direction_z = ray_direction.getVectZ();

        Vect sphere_center = center;
        double sphere_center_x = sphere_center.getVectX();
        double sphere_center_y = sphere_center.getVectY();
        double sphere_center_z = sphere_center.getVectZ();

        // Normalize
        double a = 1;

        double b = (2 * (ray_origin_x - sphere_center_x) * ray_direction_x)
            + (2 * (ray_origin_y - sphere_center_y) * ray_direction_y)
            + (2 * (ray_origin_z - sphere_center_z) * ray_direction_z);

        double c = pow(ray_origin_x - sphere_center_x, 2)
            + pow(ray_origin_y - sphere_center_y, 2)
            + pow(ray_origin_z - sphere_center_z, 2)
            - (radius * radius);

        double discriminant = b*b - 4*c;

        if (discriminant > 0) {
            // ray intersects

            // root closest to camera
            double root_1 = ((-1 * b - sqrt(discriminant)) / 2) - 0.000001;
            std::cout << root_1<< std::endl;
            if (root_1 > 0) {
                // root_1 is the closest positive root
                return root_1;
            } else {
                // root_2 is closest positive root.
                double root_2 = ((sqrt(discriminant) - b) / 2)  - 0.000001;
                std::cout << root_2<< std::endl;
                return root_2;
            }
        } else {
            // miss
            return -1;
        }
    }
};

Sphere::Sphere() {
    center = Vect(0, 0, 0);
    radius = 1.0;
    color = Color(0.2, 0.2, 0.2, 0);
}

Sphere::Sphere(Vect pcenter, double pradius, Color pcolor) {
    center = pcenter;
    radius = pradius;
    color = pcolor;
}

#endif
