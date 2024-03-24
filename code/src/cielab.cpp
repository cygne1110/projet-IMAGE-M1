#include "cielab.h"

PixelLAB fromRGB(unsigned int r, unsigned int g, unsigned int b) {

    colorm::Rgb tmp1(r, g, b);
    colorm::Lab tmp2(tmp1);
    PixelLAB res(tmp2.lightness(), tmp2.a(), tmp2.b());
    return res;

}

void fromLAB(PixelLAB src, uint& r, uint& g, uint& b) {

    colorm::Lab tmp1(src.l, src.a, src.b);
    colorm::Rgb tmp2(tmp1);
    r = tmp2.red8();
    g = tmp2.green8();
    b = tmp2.blue8();

}

std::ostream& operator<<(std::ostream& os, const PixelLAB& obj) {

    os << obj.l << " " << obj.a << " " << obj.b;
    return os;

}