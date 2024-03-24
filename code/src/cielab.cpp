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

void fromLAB(PixelLAB src, uint& r, uint& g, uint& b) {

    double delta = 6./29.;

    auto f = [&](const double& t) -> double {
        return t > delta ? t*t*t : 3*delta*delta*(t - 4./29.); 
    };

    // Standard D65
    double Xn = 95.0489;
    double Yn = 100;
    double Zn = 108.8840;    

    double X = Xn*f((src.l + 16.)/116. + src.a/500.);
    double Y = Yn*f((src.l + 16.)/116.);
    double Z = Zn*f((src.l + 16.)/116. - src.b/200.);

    double coef = 1./3400850.;

    double R = 2.36461385*X - 0.89654057*Y - 0.46807328*Z;
    double G = -0.51516621*X + 1.4264082*Y + 0.0887581*Z;
    double B = 0.0052037*X - 0.001440816*Y + 1.00920446*Z;

    auto linear = [&](const double& t) -> double {
        return t <= 0.0031308 ? 12.92*t : 1.055*pow(t, 1./2.4) - 0.055;
    };

    r = linear(R) > 255 ? 255 : linear(R) < 0 ? 0 : (int)linear(R);
    g = linear(G) > 255 ? 255 : linear(G) < 0 ? 0 : (int)linear(G);
    b = linear(B) > 255 ? 255 : linear(B) < 0 ? 0 : (int)linear(B);

}

std::ostream& operator<<(std::ostream& os, const PixelLAB& obj) {

    os << obj.l << " " << obj.a << " " << obj.b;
    return os;

}