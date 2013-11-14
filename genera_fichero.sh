#!/bin/sh

ARCH=SBR
EXEC=./NMF_atlas_mt.x

iter=1

for ((k=2; k<=128; k=2*k)) do
	echo "colon_${ARCH}( $iter, 1:4 ) = [ 2000 62 $k   ];" >> colon_${ARCH}.out
	iter=$((iter+1))
done

iter=1

for ((k=2; k<=128; k=2*k)) do
	echo "leukemia_${ARCH}( $iter, 1:4 ) = [ 7129 72 $k   ];" >> leukemia_${ARCH}.out
	iter=$((iter+1))
done

iter=1

for ((k=2; k<=128; k=2*k)) do
	echo "lung_${ARCH}( $iter, 1:4 ) = [ 12533 181 $k   ];" >> lung_${ARCH}.out
	iter=$((iter+1))
done

iter=1

for ((k=2; k<=128; k=2*k)) do
	echo "GCM_${ARCH}( $iter, 1:4 ) = [ 16063 280 $k   ];"  >> GCM_${ARCH}.out
	iter=$((iter+1))
done
