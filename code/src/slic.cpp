#include "slic.h"
#include "cielab.h"

std::ostream& operator<<(std::ostream& os, const ClusterCenter& obj) {

    os << obj.color << " " << obj.x << " " << obj.y;
    return os;

}

bool operator==(const PixelLAB& lhs, const PixelLAB& rhs) {
    return lhs.l == rhs.l && lhs.a == rhs.a && lhs.b == rhs.b;
}


bool operator==(const ClusterCenter& lhs, const ClusterCenter& rhs) {
    return lhs.color == rhs.color && lhs.x == rhs.x && lhs.y == rhs.y;
}

int idx(int i, int j, int w) {
    return i*w+j;
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

int* SLIC(ImageBase &src, int k, int m, std::vector<PixelLAB>& res_clusters) {
    
    // Initialization
    int height = src.getHeight();
    int width = src.getWidth();
    int N = src.getHeight()*src.getWidth();
    int S = ceil(sqrt((double)N/k));

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

        // std::cout << "Iteration " << iter << ", max error: " << max_error << "\n";
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

    for(int i = 0; i < k; i++) {
        res_clusters.push_back(centers[i].color);
    }

    // Fusion des superpixels similaires
    std::vector<ClusterCenter> mergedCenters = mergeSuperpixels(labels, centers, 10, N);

    // Ajout des centres fusionnés à res_clusters
    for (const auto& center : mergedCenters) {
        res_clusters.push_back(center.color);
    }

    // Créer un tableau pour stocker les labels des superpixels fusionnés
    int* merged_labels = new int[N];

    // Remplacer les labels originaux par les labels des superpixels fusionnés
    for (int i = 0; i < N; ++i) {
        int label = labels[i];
        if (merged_labels[label] == -1) {
            // Trouver le label correspondant dans les centres fusionnés
            for (size_t j = 0; j < mergedCenters.size(); ++j) {
                if (centers[label] == mergedCenters[j]) {
                    merged_labels[label] = static_cast<int>(j); // Utiliser l'index dans les centres fusionnés comme label
                    break;
                }
            }
        }
    }

    delete [] centers;
    delete [] imageLAB;
    delete [] distances;
    delete [] clusters;

    return merged_labels;
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

void get_compressed(ImageBase &src, int labels[], std::vector<PixelLAB>& clusters, int N, int k, ImageBase &res) {

    int width = src.getWidth();
    int height = src.getHeight();

    for(int i = 0; i < height; i++) {

        for(int j = 0; j < width; j++) {

            int id = idx(i, j, width);

            uint r, g, b;
            fromLAB(clusters[labels[id]], r, g, b);

            res[i*3][j*3] = r;
            res[i*3][j*3+1] = g;
            res[i*3][j*3+2] = b;

        }

    }

}

void draw_regions(ImageBase &src, int labels[], std::vector<PixelLAB>& clusters, int N, int k) {

    int width = src.getWidth();
    int height = src.getHeight();

    ImageBase res(width, height, true);

    for(int i = 0; i < height; i++) {

        for(int j = 0; j < width; j++) {

            int id = idx(i, j, width);

            uint r, g, b;
            fromLAB(clusters[labels[id]], r, g, b);

            res[i*3][j*3] = r;
            res[i*3][j*3+1] = g;
            res[i*3][j*3+2] = b;

        }

    }

    res.save("out/regions.ppm");

}

// Method to merge similar superpixels
std::vector<ClusterCenter> mergeSuperpixels(int* labels, ClusterCenter* centers, double threshold, int N) {
    std::vector<std::vector<int>> mergedClusters; // Stocke les indices des superpixels fusionnés
    std::vector<bool> merged(N, false); // Indique si un superpixel a été fusionné

    for (int i = 0; i < N; ++i) {
        if (!merged[i]) {
            std::vector<int> mergedCluster; // Stocke les indices des superpixels fusionnés dans cette itération
            mergedCluster.push_back(i); // Ajoute le superpixel actuel au cluster fusionné
            merged[i] = true; // Marque le superpixel comme fusionné

            // Parcourez les superpixels restants pour voir s'ils sont similaires
            for (int j = i + 1; j < N; ++j) {
                // Vérifiez la similarité entre les superpixels
                if (!merged[j] && distance2(centers[labels[i]], centers[labels[j]]) < threshold) {
                    mergedCluster.push_back(j); // Ajoute le superpixel à ce cluster fusionné
                    merged[j] = true; // Marque le superpixel comme fusionné
                }
            }

            // Ajoute le cluster fusionné à la liste des clusters fusionnés
            mergedClusters.push_back(mergedCluster);
        }
    }

    // Fusionne les superpixels en utilisant les moyennes des couleurs et des positions
    std::vector<ClusterCenter> mergedCenters;
    for (const auto& mergedCluster : mergedClusters) {
        ClusterCenter mergedCenter = ClusterCenter(PixelLAB(0., 0., 0.), 0., 0.);

        for (int idx : mergedCluster) {
            mergedCenter.color.l += centers[labels[idx]].color.l;
            mergedCenter.color.a += centers[labels[idx]].color.a;
            mergedCenter.color.b += centers[labels[idx]].color.b;
            mergedCenter.x += centers[labels[idx]].x;
            mergedCenter.y += centers[labels[idx]].y;
        }

        mergedCenter.color.l /= mergedCluster.size();
        mergedCenter.color.a /= mergedCluster.size();
        mergedCenter.color.b /= mergedCluster.size();
        mergedCenter.x /= mergedCluster.size();
        mergedCenter.y /= mergedCluster.size();

        mergedCenters.push_back(mergedCenter);
    }

    return mergedCenters;
}

void draw_merged_regions(ImageBase &src, int labels[], std::vector<ClusterCenter>& mergedCenters, int N, int K) {
    int width = src.getWidth();
    int height = src.getHeight();

    ImageBase res(width, height, true);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int id = idx(i, j, width);
            uint r, g, b;
            int label = labels[id];
            fromLAB(mergedCenters[label].color, r, g, b);
            res[i*3][j*3] = r;
            res[i*3][j*3+1] = g;
            res[i*3][j*3+2] = b;
        }
    }

    res.save("out/merged_regions.ppm");
}