
#include <stdio.h>
#include <iostream>
#include "include/image_ppm.h"
#include "Vec3.h"
#include <vector>

void printVec(Vec3 A){
    std::cout << '(' << A[0] << ',' << A[1] << ',' << A[2] << ')' << std::endl;
}
bool isIn(std::vector<Vec3> vec, Vec3 color){
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (vec[i]==color)return true;
    }
    return false;
}
bool isIn(std::vector<int> vec, int color){
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (vec[i]==color)return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    char cNomImgLue[250], cNomImgLue2[250];
    int nH, nW, nTaille;
    Vec3 EQM= Vec3(0,0,0);

    if (argc != 3) 
        {
        printf("Usage: Image.ppm Image2.ppm\n"); 
        exit (1) ;
        }
    
    sscanf (argv[1],"%s",cNomImgLue);
    sscanf (argv[2],"%s",cNomImgLue2);

    OCTET *ImgIn, *ImgIn2;
    
    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &nH, &nW);
    
    nTaille = nH * nW;
    int nTaille3 = nTaille * 3;
    
    allocation_tableau(ImgIn, OCTET, nTaille3);
    lire_image_ppm(cNomImgLue, ImgIn, nH * nW);
    
    allocation_tableau(ImgIn2, OCTET, nTaille3);
    lire_image_ppm(cNomImgLue2, ImgIn2, nH * nW);
    
    for (int i=0; i < nTaille3; i+=3)
    {
        EQM[0]+=pow(ImgIn[i]-ImgIn2[i],2);
        EQM[1]+=pow(ImgIn[i+1]-ImgIn2[i+1],2);
        EQM[2]+=pow(ImgIn[i+2]-ImgIn2[i+2],2);
    }
    EQM=EQM/nTaille;
    
    float PSNR = 10*log10(pow(255,2)*3/(EQM[0]+EQM[1]+EQM[2]));
    std::cout << "Le PSNR est de "<< PSNR << " décibels" << std::endl;
    
    free(ImgIn);free(ImgIn2);
    
    return 0;
}
