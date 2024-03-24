set terminal png
set output 'psnr.png'
set title "Courbe de distorsion selon le nombre de superpixels"
set xlabel "Nombre de superpixels"
set ylabel "PSNR (dB)"
set key fixed right bottom
set key nobox
plot 'out/test40.dat' with lines title "m = 40" lt rgb 'red', \
'out/test1.dat' with lines title "m = 1" lt rgb 'blue'
