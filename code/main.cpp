#include "slic.h"
#include "cielab.h"

#include <chrono>
#include <fstream>

#define CHRONO

double PSNR(ImageBase& i1, ImageBase& i2) {

    int size = i1.getHeight()*i2.getWidth();
    double EQMr = 0;
    double EQMg = 0;
    double EQMb = 0;

    for(int _i = 0; _i < i1.getHeight(); _i++) {
        for(int _j = 0; _j < i2.getWidth(); _j++) {

            int i = _i*3;
            int j = _j*3;

            EQMr += ((i1[i][j] - i2[i][j])*(i1[i][j] - i2[i][j]))/(double)size;
            EQMg += ((i1[i][j+1] - i2[i][j+1])*(i1[i][j+1] - i2[i][j+1]))/(double)size;
            EQMb += ((i1[i][j+2] - i2[i][j+2])*(i1[i][j+2] - i2[i][j+2]))/(double)size;

        }
    }

    double EQM = (EQMr + EQMg + EQMb)/3;
    double PSNR = 10 * log10(255*255/EQM);

    return PSNR;

}

int main(int argc, char* argv[]) {

    char filename[256];

    bool test = false;

    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << " image.ppm nb_superpîxels max_color_distance\n";
        std::cout << "\tor: " << argv[0] << " test image.ppm\n";
        exit(EXIT_FAILURE);
    }

    char is_test[128];
    sscanf(argv[1], "%s", is_test);
    if(strcmp(is_test, "test") == 0) test = true;

    if(test) {

        sscanf(argv[2], "%s", filename);

        ImageBase src;
        src.load(filename);

        ImageBase res(src.getWidth(), src.getHeight(), true);

        if(src.getHeight() != src.getWidth()) {
            std::cout << "Non-square images have not been tested yet, not recommended.\n";
            exit(EXIT_FAILURE);
        }

        std::ofstream data("out/test50.dat", std::ios::trunc);

        int size = src.getHeight()*src.getWidth();
        for(int K = size; K > 1; K /= 4) {

            int m = 50;
            std::vector<PixelLAB> centers;
            int* labels = SLIC(src, K, m, centers);
            get_compressed(src, labels, centers, size, K, res);
            double psnr = PSNR(src, res);

            data << K << " " << psnr << "\n";

            delete [] labels;

        }

        data.close();

    } else {

        int K, m;

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

    }

    return 0;

}
