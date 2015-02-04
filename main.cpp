#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Source.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"

using namespace std;

struct RGBType {
    double red;
    double green;
    double blue;
};

// quick hack to save bmps
void savebmp(
             const char *filename,
             int width, int height,
             int dpi,
             RGBType *data
             ) {

    FILE *f;
    int k = width * height;
    int s = 4 * k;
    int filesize = 54 + s;

    double factor = 39.375;
    int m = static_cast<int>(factor);

    int ppm = dpi * m;

    unsigned char bmpfileheader[14] = {
        'B',
        'M',
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        54,
        0,
        0,
        0
    };

    unsigned char bmpinfoheader[40] = {
        40,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        24,
        0
    };

    bmpfileheader[ 2] = (unsigned char) (filesize);
    bmpfileheader[ 3] = (unsigned char) (filesize >> 8);
    bmpfileheader[ 4] = (unsigned char) (filesize >> 16);
    bmpfileheader[ 5] = (unsigned char) (filesize >> 24);

    bmpinfoheader[ 4] = (unsigned char) (width);
    bmpinfoheader[ 5] = (unsigned char) (width >> 8);
    bmpinfoheader[ 6] = (unsigned char) (width >> 16);
    bmpinfoheader[ 7] = (unsigned char) (width >> 24);

    bmpinfoheader[ 8] = (unsigned char) (height);
    bmpinfoheader[ 9] = (unsigned char) (height >> 8);
    bmpinfoheader[10] = (unsigned char) (height >> 16);
    bmpinfoheader[11] = (unsigned char) (height >> 24);

    bmpinfoheader[21] = (unsigned char) (s);
    bmpinfoheader[22] = (unsigned char) (s >> 8);
    bmpinfoheader[23] = (unsigned char) (s >> 16);
    bmpinfoheader[24] = (unsigned char) (s >> 24);

    bmpinfoheader[25] = (unsigned char) (ppm);
    bmpinfoheader[26] = (unsigned char) (ppm >> 8);
    bmpinfoheader[27] = (unsigned char) (ppm >> 16);
    bmpinfoheader[28] = (unsigned char) (ppm >> 24);

    bmpinfoheader[29] = (unsigned char) (ppm);
    bmpinfoheader[30] = (unsigned char) (ppm >> 8);
    bmpinfoheader[31] = (unsigned char) (ppm >> 16);
    bmpinfoheader[32] = (unsigned char) (ppm >> 24);

    f = fopen(filename, "wb");

    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);

    for(int i = 0; i < k; i++) {
        RGBType rgb = data[i];

        double red = (data[i].red) * 255;
        double green = (data[i].green) * 255;
        double blue = (data[i].blue) * 255;

        unsigned char color[3] = {
            (int) floor(blue),
            (int) floor(green),
            (int) floor(red)
        };

        fwrite(color, 1, 3, f);
    }

    fclose(f);

}

int winningObjectIndex(vector<double> object_intersections) {
    int index_of_min_value;

    if (object_intersections.size() == 0) {
        // The object isn't in the field of view.
        return -1;
    } else if (object_intersections.size() == 1) {

        if (object_intersections.at(0) > 0) {
            // if the intersection is > 0 then it's or index of min value.
            return 0;
        } else {
            //miss, negative intersection
            return -1;
        }
    } else {
        // which intersection is closer?
        double max = 0.0;

        for (int i = 0; i < object_intersections.size(); i++) {
            if (max < object_intersections.at(i)) {
                max = object_intersections.at(i);
            }
        }

        if (max > 0) {
            // we only care about positive numbers.
            for (int index = 0; index < object_intersections.size(); index++) {

                if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {
                    max = object_intersections.at(index);
                    index_of_min_value = index;
                }
            }

            return index_of_min_value;
        } else {
            // all intersections are negative.
            return -1;
        }
    }
};

Color getColorAt(
    Vect intersection_position,
    Vect intersecting_ray_direction,
    vector<Object*> scene_objects,
    int index_of_winning_object,
    vector<Source*> light_sources,
    double accuracy,
    double ambient_light
) {

    Color winning_object_color = scene_objects.at(
        index_of_winning_object
    )->getColor();

    Vect winning_object_normal = scene_objects.at(
        index_of_winning_object
    )->getNormalAt(intersection_position);

    if (winning_object_color.getColorSpecial() == 2) {
        //tiles
        int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());

        if ((square %2) == 0 ) {
            winning_object_color.setColorRed(0);
            winning_object_color.setColorGreen(0);
            winning_object_color.setColorBlue(0);
            winning_object_color.setColorSpecial(0.3);
        } else {
            winning_object_color.setColorSpecial(0.3);
        }
    }

    Color final_color = winning_object_color.colorScale(ambient_light);

    if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
        //reflection of objects with specular intensity.
        double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.invert());
        Vect scalar1 = winning_object_normal.vectScale(dot1);
        Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
        Vect scalar2 = add1.vectScale(2);
        Vect add2 = intersecting_ray_direction.invert().vectAdd(scalar2);
        Vect reflection_direction = add2.normalize();

        Ray reflection_ray(intersection_position, reflection_direction);

        vector<double> reflection_intersections;

        for (
             int reflection_index = 0;
             reflection_index < scene_objects.size();
             reflection_index++
        ) {
            reflection_intersections.push_back(
                scene_objects.at(reflection_index)->findIntersection(reflection_ray)
            );
        }

        int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

        if (index_of_winning_object_with_reflection != -1) {
            // reflection ray miss
            if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
                // find reflection color
                Vect reflection_intersection_position = intersection_position.vectAdd(
                    reflection_direction.vectScale(
                        reflection_intersections.at(index_of_winning_object_with_reflection)
                    )
                );

                Vect reflection_intersection_ray_direction = reflection_direction;
                Color reflection_intersection_color = getColorAt(
                                                          reflection_intersection_position,
                                                          reflection_intersection_ray_direction,
                                                          scene_objects,
                                                          index_of_winning_object_with_reflection,
                                                          light_sources,
                                                          accuracy,
                                                          ambient_light
                                                      );
                final_color = final_color.colorAdd(winning_object_color.colorScale(winning_object_color.getColorSpecial()));
            }
        }
    }

    for (int light_index = 0; light_index < light_sources.size(); light_index++) {

        Vect light_direction = light_sources.at(
            light_index
        )->getLightPosition().vectAdd(
            intersection_position.invert()
        ).normalize();

        float cosine_angle = winning_object_normal.dotProduct(light_direction);

        if (cosine_angle > 0) {
            // test for shadows
            bool shadowed = false;

            Vect distance_to_light = light_sources.at(
                light_index
            )->getLightPosition().vectAdd(
                intersection_position.invert()
            ).normalize();

            float distance_to_light_magnitude = distance_to_light.magnitude();

            Ray shadow_ray (
                intersection_position,
                light_sources.at(light_index)->getLightPosition().vectAdd(
                    intersection_position.invert()
                ).normalize()
            );

            vector<double> secondary_intersections;

            for (
                 int object_index = 0;
                 object_index < scene_objects.size() && shadowed == false;
                 object_index++
            ) {
                secondary_intersections.push_back(
                    scene_objects.at(object_index)->findIntersection(shadow_ray)
                );
            }

            for(int c = 0; c < secondary_intersections.size(); c++) {
                if (secondary_intersections.at(c) > accuracy) {
                    if (secondary_intersections.at(c) <= distance_to_light_magnitude ) {
                        shadowed = true;
                    }
                    break;
                }
            }

            if (shadowed == false) {
                final_color = final_color.colorAdd(
                    winning_object_color.colorMultiply(
                        light_sources.at(light_index)->getLightColor()
                    ).colorScale(cosine_angle)
                );

                if (
                    winning_object_color.getColorSpecial() > 0
                    && winning_object_color.getColorSpecial() <= 1
                ) {
                    // special 0...1 range
                    double dot1 = winning_object_normal.dotProduct(
                        intersecting_ray_direction.invert()
                    );

                    Vect scalar1 = winning_object_normal.vectScale(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectScale(2);
                    Vect add2 = intersecting_ray_direction.invert().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();

                    double specular = reflection_direction.dotProduct(
                        light_direction
                    );

                    if (specular > 0 ) {

                        specular = pow(specular, 10);
                        final_color = final_color.colorAdd(
                            light_sources.at(light_index)->getLightColor().colorScale(
                                specular * winning_object_color.getColorSpecial()
                            )
                        );
                    }
                }
            }
        }
    }

    return final_color.clip();
};

int current_pixel;

int main (int argc, char *argv[]) {

    //Rendering output
    int dpi = 401;
    int width = 1280; // in pixels
    int height = 720;
    int n = width * height; //total pixels
    RGBType *pixels = new RGBType[n];

    double aspect_ratio = (double)width / (double)height;
    double ambient_light  = 0.6;
    double accuracy = 0.0000001;

    // Scene.
    Vect O (0, 0, 0); //Origin
    Vect X (1, 0, 0);
    Vect Y (0, 1, 0);
    Vect Z (0, 0, 1);

    Vect cam_pos(6, 1.5, -4);
    // Look at the scene origin or O.
    Vect look_at(O);

    // Difference between the camera and what it's looking at.
    Vect diff_btw(
      cam_pos.getVectX() - look_at.getVectX(),
      cam_pos.getVectY() - look_at.getVectY(),
      cam_pos.getVectZ() - look_at.getVectZ()
    );

    // Camera
    Vect cam_dir = diff_btw.invert().normalize();
    Vect cam_right = Y.crossProduct(cam_dir).normalize();
    Vect cam_down = cam_right.crossProduct(cam_dir);

    Camera scene_camera(cam_pos, cam_dir, cam_right, cam_down);

    // Some simple colors.
    Color white_light(1.0, 1.0, 1.0, 0);
    Color pretty_green(0.5, 1.0, 0.5, 0.3);
    Color maroon(0.5, 0.25, 0.25, 2);
    Color gray(0.5, 0.5, 0.5, 0);
    Color black(0.0, 0.0, 0.0, 0);
    Color sky_blue(0.52, 0.8, 0.92, 0);

    // Lighting.
    Vect light_position(-12, 10, -10);
    Light scene_light(light_position, white_light);
    vector<Source*> light_sources;
    light_sources.push_back(dynamic_cast<Source*>(&scene_light));

    // Create objects.
    Sphere sphere_1(Vect(0, 0, 0), 1, pretty_green);
    Plane scene_plane(Y, -1, maroon);

    // Add objects to scene.
    vector<Object*> scene_objects;
    scene_objects.push_back(dynamic_cast<Object*>(&sphere_1));
    scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));

    double xamnt, yamnt;

    cout << "Rendering..." << endl;

    // Rasterizer

    // loop every pixel from left to right.
    for(int x = 0; x < width; x++) {
        // loop every pixel from top to bottom.
        for(int y = 0; y < height; y++) {

            // If we are on y = 200 and x = 175 on a scene 400 pixels wide:
            // 200*400+175 = 80175
            current_pixel = y * width + x;

            // Determine if the image is landscape, portrait, or square.
            // We use this to determine what direction all the rays that leave
            // the camera will have.
            if (width > height) {
                // the image is wide.
                xamnt = ((x + 0.5) / width) * aspect_ratio - (((width - height ) / (double) height) / 2);
                yamnt = ((height - y) + 0.5) / height;
            } else if (width < height) {
                // the image is tall.
                xamnt = (x + 0.5) / width;
                yamnt = (((height - y) + 0.5) / height) / aspect_ratio - (((height - width) / (double)width) / 2);
            } else {
                // the image is a square.
                xamnt = (x + 0.5) / width;
                yamnt = ((height - y) + 0.5) / height;
            }

            // The origin of the tracer ray is the camera's current position.
            // The Camera direction is determined by using some linear algebra
            // on it's right and down properties.
            Vect camera_ray_origin = scene_camera.getCameraPosition();

            // A ray shoots out of the camera in a direction dependent on it's
            // location. The entire grid of rays that leave the camera will have
            // to scale up to the vector that lays in front of it (the scene).
            Vect camera_ray_direction = cam_dir.vectAdd(
              cam_right.vectScale(xamnt - 0.5).vectAdd(cam_down.vectScale(yamnt - 0.5))
            ).normalize();

            // The tracer ray that is fire at each pixel.
            Ray cam_ray(camera_ray_origin, camera_ray_direction);

            // Build an index of all intersections in the scene.
            vector<double> intersections;

            for(int index = 0; index < scene_objects.size(); index++) {
                intersections.push_back(
                  scene_objects.at(index)->findIntersection(cam_ray)
                );

            }

            // object closest to the camera
            int index_of_winning_object = winningObjectIndex(intersections);

            // For a miss, draw the background.
            if (index_of_winning_object == -1) {
                pixels[current_pixel].red = sky_blue.getColorRed();
                pixels[current_pixel].green = sky_blue.getColorGreen();
                pixels[current_pixel].blue = sky_blue.getColorBlue();
            } else {
                // We Have a hit on this pixel.

                // The color of the pixel at the intersection of the tracer ray and object.
                if (intersections.at(index_of_winning_object) > accuracy) {
                    Vect intersection_position = camera_ray_origin.vectAdd(
                      camera_ray_direction.vectScale(intersections.at(index_of_winning_object))
                    );

                    Vect intersecting_ray_direction = camera_ray_direction;

                    Color intersection_color = getColorAt(
                      intersection_position,
                      intersecting_ray_direction,
                      scene_objects,
                      index_of_winning_object,
                      light_sources,
                      accuracy,
                      ambient_light
                    );


                    //if (scene_objects.at(index_of_winning_object)->circle()) {
                    //   cout << "R: " << this_color.getColorRed();
                    //   cout << "G: " << this_color.getColorGreen();
                    //   cout << "B: " << this_color.getColorBlue();
                    // }
                    pixels[current_pixel].red = intersection_color.getColorRed();
                    pixels[current_pixel].green = intersection_color.getColorGreen();
                    pixels[current_pixel].blue =intersection_color.getColorBlue();
                }
            }
        }


    }

    delete pixels;

    savebmp("scene.bmp", width, height, dpi, pixels);

    return 0;
}
