#pragma once

#include "cielab.h"
#include "ImageBase.h"

#include <cstring>
#include <cmath>
#include <vector>

#define MINCONV 0.01f
#define BORDER_R 200
#define BORDER_G 200
#define BORDER_B 0

struct ClusterCenter {

    PixelLAB color;
    double x, y;

    ClusterCenter(): x(0), y(0) {};
    ClusterCenter(PixelLAB _color, unsigned int _x, unsigned int _y): color(_color), x(_x), y(_y) {};
    ClusterCenter(const ClusterCenter& other) {
        color = other.color; x = other.x; y = other.y;
    }

    ClusterCenter& operator+=(const ClusterCenter& other) {
        color.l += other.color.l;
        color.a += other.color.a;
        color.b += other.color.b;
        x += other.x;
        y += other.y;
        return *this;
    }

    ClusterCenter& operator/=(const double& other) {
        color.l /= other;
        color.a /= other;
        color.b /= other;
        x /= other;
        y /= other;
        return *this;
    }

};

std::ostream& operator<<(std::ostream& os, const PixelLAB& obj);

// Computes the distance between two CIELAB pixels for SLIC
double distance(ClusterCenter, PixelLAB, int, int, int, int);

// Regroups the pixels of an image into superpixels using the SLIC algorithm
// - Return a 2D array of labels, pixel(i, j) is contained in the superpixel labels[i][j]
// - Takes an images src, k the number of the superpixels and m the user defined max color distance
int* SLIC(ImageBase&, int, int, std::vector<PixelLAB>&);

void superpixels(ImageBase&, int[], int);
void draw_regions(ImageBase&, int[], std::vector<PixelLAB>&, int, int);
void get_compressed(ImageBase &src, int labels[], std::vector<PixelLAB>& clusters, int N, int k, ImageBase &res);
