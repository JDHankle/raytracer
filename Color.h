#ifndef _COLOR_H
#define _COLOR_H

class Color {
    double red, green, blue, special;

 public:

    Color ();
    Color (double, double, double, double);

    double getColorRed() { return red; }
    double getColorGreen() { return green; }
    double getColorBlue() { return blue; }
    double getColorSpecial() { return special; }

    double setColorRed(double pred) { red = pred; }
    double setColorGreen(double pgreen) { green = pgreen; }
    double setColorBlue(double pblue) { blue = pblue; }
    double setColorSpecial(double pspecial) { special = pspecial; }

    double brightness() {
        return(red + green + blue) / 3;
    }

    Color colorScale(double scalar) {
        return Color(red * scalar, green * scalar, blue * scalar, special);
    }

    Color colorAdd(Color color) {
        return Color(
          red + color.getColorRed(),
          green + color.getColorGreen(),
          blue + color.getColorBlue(),
          special
        );
    }

    Color colorMultiply(Color color) {
        return Color(
          red * color.getColorRed(),
          green * color.getColorGreen(),
          blue * color.getColorBlue(),
          special
        );
    }

    Color colorAverage(Color color) {
        return Color(
          (red * color.getColorRed()) / 2,
          (green * color.getColorGreen()) / 2,
          (blue * color.getColorBlue()) / 2,
          special
        );

    }

    Color clip() {
        double all_light = red + green + blue;
        double extra_light = all_light - 3;
        if (extra_light > 0) {
            red = red + extra_light*(red/all_light);
            green = green + extra_light*(green/all_light);
            blue = blue + extra_light*(blue/all_light);
        }

        if (red > 1) { red = 1; };
        if (green > 1) { green = 1; };
        if (blue > 1) { blue = 1; };

        if (red < 0) { red = 0; };
        if (green < 0) { green = 0; };
        if (blue < 0) { blue = 0; };

        return Color(red, green, blue, special);
    }
};

Color::Color() {
    red = 0.5;
    green = 0.5;
    blue = 0.5;
}

Color::Color(double pred, double pgreen, double pblue, double pspecial) {
    red = pred;
    green = pgreen;
    blue = pblue;
    special = pspecial;
}

#endif
