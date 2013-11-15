#!/bin/bash

EXEC=./NMF_st_float.x

for (( k=2; k<=128; k=2*k )) 
do
	$EXEC none.bin 2000 62 $k 1 40         >> out/cpu_st_float__2000x62_${k};
	$EXEC none.bin 7129 77 $k 1 40         >> out/cpu_st_float__7129x77_${k};
	$EXEC none.bin 12533 181 $k 1 40       >> out/cpu_st_float__12533x181_${k};
	$EXEC none.bin 16063 280 $k 1 40       >> out/cpu_st_float__16063x280_${k};
done

EXEC=./NMF_mt_float.x

for (( k=2; k<=128; k=2*k )) 
do
	$EXEC none.bin 2000 62 $k 1 40         >> out/cpu_mt_float__2000x62_${k};
	$EXEC none.bin 7129 77 $k 1 40         >> out/cpu_mt_float__7129x77_${k};
	$EXEC none.bin 12533 181 $k 1 40       >> out/cpu_mt_float__12533x181_${k};
	$EXEC none.bin 16063 280 $k 1 40       >> out/cpu_mt_float__16063x280_${k};
done

EXEC=./NMF_st_double.x

for (( k=2; k<=128; k=2*k )) 
do
	$EXEC none.bin 2000 62 $k 1 40         >> out/cpu_st_double__2000x62_${k};
	$EXEC none.bin 7129 77 $k 1 40         >> out/cpu_st_float__7129x77_${k};
	$EXEC none.bin 12533 181 $k 1 40       >> out/cpu_st_float__12533x181_${k};
	$EXEC none.bin 16063 280 $k 1 40       >> out/cpu_st_float__16063x280_${k};
done

EXEC=./NMF_mt_double.x

for (( k=2; k<=128; k=2*k )) 
do
	$EXEC none.bin 2000 62 $k 1 40         >> out/cpu_mt_double__2000x62_${k};
	$EXEC none.bin 7129 77 $k 1 40         >> out/cpu_mt_float__7129x77_${k};
	$EXEC none.bin 12533 181 $k 1 40       >> out/cpu_mt_float__12533x181_${k};
	$EXEC none.bin 16063 280 $k 1 40       >> out/cpu_mt_float__16063x280_${k};
done
