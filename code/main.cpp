#include "slic.h"
#include "cielab.h"

#include <chrono>

#define CHRONO

int main(int argc, char* argv[]) {

    char filename[256];
    int K, m;

    if(argc != 4) {
        std::cout << "Usage: " << argv[0] << " image.ppm nb_superpîxels max_color_distance\n";
        exit(EXIT_FAILURE);
    }

    sscanf(argv[1], "%s", filename);
    sscanf(argv[2], "%d", &K);
    sscanf(argv[3], "%d", &m);
    
    ImageBase src;
    src.load(filename);

    if(src.getHeight() != src.getWidth()) {
        std::cout << "Non-square images have not been tested yet, not recommended.\n";
        exit(EXIT_FAILURE);
    }

    // if(K > src.getHeight()) {
    //     std::cout << "The number of superpixels should be lesser of equal to the width of the image.\n";
    //     exit(EXIT_FAILURE);
    // }

    if(m <= 0) {
        std::cout << "The max color distance must be positive. (Values ranging from 1 to 40 work best)\n";
        exit(EXIT_FAILURE);
    }

    int N = src.getHeight()*src.getWidth();
    std::vector<PixelLAB> centers;

    std::cout << "Hello SLIC!\n";

    #ifdef CHRONO
    auto start = std::chrono::system_clock::now();
    #endif
    int* labels = SLIC(src, K, m, centers);
    #ifdef CHRONO
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    #endif
    std::cout << "Segmentation complete";
    #ifdef CHRONO
    std::cout << " in " << elapsed.count() << "ms";
    #endif
    std::cout << "!\n";

    superpixels(src, labels, N);
    draw_regions(src, labels, centers, N, K);

    src.save("out/test.ppm");

    delete [] labels;

    return 0;

}
