#include "slic.h"
#include "cielab.h"

int idx(int i, int j, int w) {
    return i*w+j;
}

std::ostream& operator<<(std::ostream& os, const ClusterCenter& obj) {

    os << obj.color << " " << obj.x << " " << obj.y;
    return os;

}

PixelLAB operator+(const PixelLAB& lhs, const PixelLAB& rhs) {

    return PixelLAB(lhs.l + rhs.l, lhs.a + rhs.a, lhs.b + rhs.b);

}

PixelLAB operator-(const PixelLAB& lhs, const PixelLAB& rhs) {

    return PixelLAB(lhs.l - rhs.l, lhs.a - rhs.a, lhs.b - rhs.b);

}

double distance(ClusterCenter C, PixelLAB i, int _x, int _y, int S, int m) {

    double l = C.color.l - i.l;
    double a = C.color.a - i.a;
    double b = C.color.b - i.b;
    double x = C.x - _x;
    double y = C.y - _y;

    double color_distance = sqrt(l*l + a*a + b*b);
    double spatial_distance = sqrt(x*x + y*y);

    return sqrt(color_distance*color_distance + (spatial_distance/S)*(spatial_distance/S)*m*m);

}

double distance2(ClusterCenter c1, ClusterCenter c2) {

    double l = c1.color.l - c2.color.l;
    double a = c1.color.a - c2.color.a;
    double b = c1.color.b - c2.color.b;
    double x = c1.x - c2.x;
    double y = c1.y - c2.y;

    return l*l + a*a + b*b + x*x + y*y;

}

double distance2(PixelLAB p1, PixelLAB p2) {

    double l = p1.l - p2.l;
    double a = p1.a - p2.a;
    double b = p1.b - p2.b;

    return l*l + a*a + b*b;

}

int* SLIC(ImageBase &src, int k, int m) {
    
    // Initialization
    int height = src.getHeight();
    int width = src.getWidth();
    int N = src.getHeight()*src.getWidth();
    int S = floor(sqrt((double)N/k)); // I think that this needs to be a power of 2 to work

    int* labels = new int[N];
    double* distances = new double[N];
    PixelLAB* imageLAB = new PixelLAB[N];
    ClusterCenter* centers = new ClusterCenter[k];
    std::vector<int>* clusters = new std::vector<int>[k];

    memset(labels, -1, N*sizeof(int));

    // CIELAB image from RGB image
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int r = src[i*3][j*3];
            int g = src[i*3][j*3+1];
            int b = src[i*3][j*3+2];
            imageLAB[idx(i, j, width)] = fromRGB(r, g, b);
            distances[idx(i, j, width)] = std::numeric_limits<double>::max();
        }
    }

    // sampling clusters
    int p = 0;
    for(int i = 0; i < height; i += S) {
        for(int j = 0; j < width; j += S) {
            centers[p] = ClusterCenter(imageLAB[idx(i, j, width)], i, j);
            p++;
        }
    }

    // set cluster centers to the lowest gradient position in a 3x3 neighborhood
    // the code is hideous
    for(int i = 0; i < k; i++) {

        int x = centers[i].x;
        int y = centers[i].y;

        int p = 0;
        int min_pos = 0;
        double min = std::numeric_limits<double>::max();

        for(int a = -1; a <= 1; a++) {

            for(int b = -1; b <= 1; b++) {

                int _x = x+a;
                int _y = y+b;

                if(_x > 0 && _x < height-1 && _y > 0 && _y < width-1) {
                    double grad = distance2(imageLAB[idx(_x-1, _y, width)] - imageLAB[idx(_x+1, _y, width)], imageLAB[idx(_x, _y-1, width)] - imageLAB[idx(_x, _y+1, width)]);
                    if(min > grad) {
                        min = grad;
                        min_pos = p;
                    }
                }

                p++;

            }

        }

        p = 0;
        for(int a = -1; a <= 1; a++) {

            for(int b = -1; b <= 1; b++) {

                int _x = x+a;
                int _y = y+b;

                if(min_pos == p) {
                    centers[i] = ClusterCenter(imageLAB[idx(_x, _y, width)], _x, _y);
                }

                p++;

            }
            
        }

    }

    // Main loop
    int iter = 0;
    double max_error = std::numeric_limits<double>::min();
    do {
        max_error = std::numeric_limits<double>::min();

        // Assignement
        for(int c = 0; c < k; c++) {

            for(int a = -1*S; a <= S; a++) {

                int _x = floor(centers[c].x) + a;
                if(_x < 0 || _x >= height) continue;

                for(int b = -1*S; b <= S; b++) {

                    int _y = floor(centers[c].y) + b;
                    if(_y < 0 || _y >= width) continue;

                    double D = distance(centers[c], imageLAB[idx(_x, _y, width)], _x, _y, S, m);

                    if(distances[idx(_x, _y, width)] > D) {
                        distances[idx(_x, _y, width)] = D;
                        labels[idx(_x, _y, width)] = c;
                    }

                    clusters[c].push_back(idx(_x, _y, width));

                }

            }

        }

        double error;

        // Update clusters
        for(int c = 0; c < k; c++) {

            ClusterCenter mean = ClusterCenter(PixelLAB(0., 0., 0.), 0., 0.);

            int i = 0;
            int size = clusters[c].size();
            for(; i < size; i++) {

                int id = clusters[c][i];
                mean += ClusterCenter(imageLAB[id], id/width, id%width);

            }
            mean /= (double)i;

            error = distance2(mean, centers[c]);
            if(error > max_error) max_error = error;            

            centers[c] = mean;
            clusters[c].clear();

        }

        std::cout << "Iteration " << iter << ", max error: " << max_error << "\n";
        iter++;

    } while(max_error > MINCONV && iter < 15);

    // post processing, to assign labels to the orphaned pixels
    // connected component algorithm
    bool orphan = false;
    do {
        for(int i = 0; i < N; i++) {

            if(labels[i] == -1) {

                std::vector<int> neighbors;

                int x = i/width;
                int y = i%width;

                if(x > 0 && labels[idx(x-1, y, width)] != -1) neighbors.push_back(labels[idx(x-1, y, width)]);
                if(x < height-1 && labels[idx(x+1, y, width)] != -1) neighbors.push_back(labels[idx(x+1, y, width)]);
                if(y > 0 && labels[idx(x, y-1, width)] != -1) neighbors.push_back(labels[idx(x, y-1, width)]);
                if(y < width-1 && labels[idx(x, y+1, width)] != -1) neighbors.push_back(labels[idx(x, y+1, width)]);

                if(neighbors.empty()) {
                    orphan = true;
                    continue;
                }

                int min_label = 0;
                double min = std::numeric_limits<double>::max();
                int size = neighbors.size();
                for(int j = 0; j < size; j++) {

                    double dist = distance(centers[neighbors[j]], imageLAB[i], x, y, S, m);
                    if(dist < min) {
                        min = dist;
                        min_label = neighbors[j];
                    }

                }

                labels[i] = min_label;

            }

        }
    } while(orphan);

    delete [] centers;
    delete [] imageLAB;
    delete [] distances;
    delete [] clusters;
    return labels;

}

void superpixels(ImageBase &src, int labels[], int N) {

    int width = src.getWidth();
    int height = src.getHeight();

    for(int i = 0; i < height-1; i++) {

        for(int j = 0; j < width-1; j++) {

            int id = idx(i, j, width);

            if(labels[id] != labels[id+1] || labels[id] != labels[id+width] || labels[id] != labels[id+1+width]) {

                src[i*3][j*3] = (unsigned int)BORDER_R;
                src[i*3][j*3+1] = (unsigned int)BORDER_G;
                src[i*3][j*3+2] = (unsigned int)BORDER_B;

            }

        }

    }

}

void draw_regions(ImageBase &src, int labels[], int N, int k) {

    int width = src.getWidth();
    int height = src.getHeight();

    ImageBase res(width, height, false);

    for(int i = 0; i < height; i++) {

        for(int j = 0; j < width-1; j++) {

            int id = idx(i, j, width);

            res[i][j] = (labels[id]*255)/k;

        }

    }

    res.save("out/regions.ppm");

}
