#ifndef _CAMERA_H
#define _CAMERA_H

#include "Vect.h"

class Camera {
    Vect position, direction, right, down;

 public:

    Camera ();
    Camera (Vect, Vect, Vect, Vect);

    Vect getCameraPosition() { return position; };
    Vect getCameraDirection(){ return direction; };
    Vect getCameraRight(){ return right; };
    Vect getCameraDown(){ return down; };

};

Camera::Camera() {
    position = Vect(0, 0, 0);
    direction = Vect(0, 0, 1);
    right = Vect(0, 0, 0);
    down = Vect(0, 0, 0);

}

Camera::Camera(Vect pposition, Vect pdirection, Vect pright, Vect pdown) {
    position = pposition;
    direction = pdirection;
    right = pright;
    down = pdown;
}

#endif
