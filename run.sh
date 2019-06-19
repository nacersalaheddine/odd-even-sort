#!/bin/bash
#by Nacer Salah eddine


#Script Used for compiling and running openmpi programs
clear
source_file_name="./odd-even-MPI.c"
output_file_name="./bin/pgm"
no_of_process=5

echo ---------Compiling Program------------

mpicc $source_file_name  -o $output_file_name -std=c99 -Wall
if [ $? == 0 ];then #No Errors Detected
	echo "COMPILE SUCCES"
	echo ---------Executing Program------------

    mpirun --allow-run-as-root --hostfile hostfile -np $no_of_process $output_file_name
	
else
	echo "COMPILE ERROR"
fi


#END OF SCRIPT
