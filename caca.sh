#!/bin/bash

para_arriba=true
cambio_direccion=false
x_actual=0
y_actual=0

echo "15000" > /tmp/como.txt
for i in $(seq 15000)
do

	echo "$x_actual $y_actual" >> /tmp/como.txt

	x_actual=$(( x_actual + 2 ))
	if $para_arriba ;
	then
		y_actual=$(( y_actual + 16 ))
	else
		y_actual=$(( y_actual - 16 ))
	fi


#	cambio_direccion=$(( (i-1) % 2000 ))
	cambio_direccion=$(( y_actual % 32000 ))

#	echo "el cambio de direccion es $cambio_direccion" >> /tmp/como.txt

	if (( cambio_direccion==0  )) ; 
#	if [[ $cambio_direccion -eq 0  ]] ; 
	then
		echo "cambiando de direccion en x $x_actual para arriba es $para_arriba"
		if ! $para_arriba ;
		then
			para_arriba=true
		else
			para_arriba=false
		fi
	fi

done

#XXX: http://stackoverflow.com/questions/14562423/is-there-a-way-to-ignore-header-lines-in-a-unix-sort
cat /tmp/como.txt | awk 'NR<2{print $0;next}{print $0| "sort -k 2n -k 1n"}' > /tmp/como_tmp.txt 
mv -fv /tmp/como_tmp.txt /tmp/como.txt
