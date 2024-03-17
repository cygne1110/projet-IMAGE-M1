#pragma once

#include <limits>
#include <iostream>
#include <cmath>

struct PixelLAB {

    double l, a, b; // color values

    PixelLAB(): l(0.), a(0.), b(0.) {};
    PixelLAB(double _l, double _a, double _b): l(_l), a(_a), b(_b) {};
    PixelLAB(const PixelLAB& other) {
        l = other.l; a = other.a; b = other.b;
    }

};

std::ostream& operator<<(std::ostream& os, const PixelLAB& obj);

PixelLAB operator+(const PixelLAB& lhs, const PixelLAB& rhs);
PixelLAB operator-(const PixelLAB& lhs, const PixelLAB& rhs);

PixelLAB fromRGB(unsigned int, unsigned int, unsigned int);