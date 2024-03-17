#include "cielab.h"

PixelLAB fromRGB(unsigned int r, unsigned int g, unsigned int b) {

    double X = 0.49*r + 0.31*g + 0.2*b;
    double Y = 0.17697*r + 0.81240*g + 0.01063*b;
    double Z = 0.01*g + 0.99*b;

    // Standard D65
    double Xn = 95.0489;
    double Yn = 100;
    double Zn = 108.8840;

    double delta = 6./29.;

    auto f = [&](const double& t) -> double {
        return t > delta*delta*delta ? cbrt(t) : t/(3.*delta*delta) + (4./29.);
    };

    double L = 166*f(Y/Yn) - 16;
    double A = 500*(f(X/Xn) - f(Y/Yn));
    double B = 200*(f(Y/Yn) - f(Z/Zn));

    return PixelLAB(L, A, B);

}

std::ostream& operator<<(std::ostream& os, const PixelLAB& obj) {

    os << obj.l << " " << obj.a << " " << obj.b;
    return os;

}