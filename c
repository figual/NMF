ATLAS_INCLUDE=/home/figual/atlas/include
ATLAS_LIB=/home/figual/atlas/lib

# SP
#gcc -I$ATLAS_INCLUDE -o NMF_catlas_st_float.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DREAL -DBLAS -DATLAS $ATLAS_LIB/libcblas.a $ATLAS_LIB/libatlas.a -lm
#gcc -I$ATLAS_INCLUDE -o NMF_catlas_mt_float.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DREAL -DBLAS -DATLAS $ATLAS_LIB/libptcblas.a $ATLAS_LIB/libatlas.a -lpthread -lm

gcc -I$ATLAS_INCLUDE -o NMF_fatlas_st_float.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DREAL -DBLAS -DFATLAS $ATLAS_LIB/libf77blas.a $ATLAS_LIB/libatlas.a -lm -lgfortran
gcc -I$ATLAS_INCLUDE -o NMF_fatlas_mt_float.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DREAL -DBLAS -DFATLAS $ATLAS_LIB/libptf77blas.a $ATLAS_LIB/libatlas.a -lpthread -lm -lgfortran

# DP
#gcc -I$ATLAS_INCLUDE -o NMF_catlas_st_double.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DBLAS -DATLAS $ATLAS_LIB/libcblas.a $ATLAS_LIB/libatlas.a -lm
#gcc -I$ATLAS_INCLUDE -o NMF_catlas_mt_double.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DBLAS -DATLAS $ATLAS_LIB/libptcblas.a $ATLAS_LIB/libatlas.a -lpthread -lm

gcc -I$ATLAS_INCLUDE -o NMF_fatlas_st_double.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DBLAS -DFATLAS $ATLAS_LIB/libf77blas.a $ATLAS_LIB/libatlas.a -lm -lgfortran
gcc -I$ATLAS_INCLUDE -o NMF_fatlas_mt_double.x NMF_LeeSeung.c -lm -DRANDOM -O3 -DBLAS -DFATLAS $ATLAS_LIB/libptf77blas.a $ATLAS_LIB/libatlas.a -lpthread -lm -lgfortran
