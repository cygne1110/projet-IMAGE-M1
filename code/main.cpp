#include "slic.h"
#include "cielab.h"

int main(int argc, char* argv[]) {

    char filename[256];
    int K, m;

    if(argc != 4) {
        std::cout << "Usage: " << argv[0] << " image.pgm nb_superpîxels max_color_distance\n";
        exit(EXIT_FAILURE);
    }

    sscanf(argv[1], "%s", filename);
    sscanf(argv[2], "%d", &K);
    sscanf(argv[3], "%d", &m);

    if(!(K == pow(4, (int)(log(K)/log(4)) && K != 0))) {
        std::cout << "The number of superpixels should be a power of 4.\n";
        exit(EXIT_FAILURE);
    }

    ImageBase src;
    src.load(filename);

    if(src.getHeight() != src.getWidth()) {
        std::cout << "Non-square images have not been tested yet, not recommended.\n";
        exit(EXIT_FAILURE);
    }

    if(K >= src.getHeight()) {
        std::cout << "The number of superpixels should be less than the width of the image.\n";
        exit(EXIT_FAILURE);
    }

    if(m <= 0) {
        std::cout << "The max color distance must be positive. (Values ranging from 1 to 40 work best)\n";
        exit(EXIT_FAILURE);
    }

    int N = src.getHeight()*src.getWidth();

    std::cout << "Hello SLIC!\n";

    int* labels = SLIC(src, K, m);

    std::cout << "Segmentation complete!\n";

    superpixels(src, labels, N);
    draw_regions(src, labels, N, K);

    src.save("out/test.ppm");

    delete [] labels;

    return 0;

}