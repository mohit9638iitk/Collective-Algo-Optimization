
rm -f plot.txt;
for execution in {1..10};
do

	#P=4 and ppn=1
	python script.py 2 2 1 ;
	mpirun -n 4 -f hostfile ./src 16 >> plot.txt ;
	python script.py 2 2 1 ;
	mpirun -n 4 -f hostfile ./src 256 >> plot.txt ;
	python script.py 1 4 1 ;
	mpirun -n 4 -f hostfile ./src 2048 >> plot.txt ;
	
	#P=4 and ppn=8
	python script.py 2 2 8 ;
	mpirun -n 32 -f hostfile ./src 16 >> plot.txt ;
	python script.py 4 4 1 ;
	mpirun -n 32 -f hostfile ./src 256 >> plot.txt ;
	python script.py 4 1 8 ;
	mpirun -n 32 -f hostfile ./src 2048 >> plot.txt ;
	
	#P=16 and ppn=1
	python script.py 4 4 1 ;
	mpirun -n 16 -f hostfile ./src 16 >> plot.txt ;
	python script.py 4 4 1 ;
	mpirun -n 16 -f hostfile ./src 256 >> plot.txt ;
	python script.py 4 4 1 ;
	mpirun -n 16 -f hostfile ./src 2048 >> plot.txt ;

	#P=16 and ppn=8
	python script.py 4 4 8;
	mpirun -n 128 -f hostfile ./src 16 >> plot.txt ;
	python script.py 4 4 8 ;
	mpirun -n 128 -f hostfile ./src 256 >> plot.txt ;
	python script.py 4 4 8 ;
	mpirun -n 128 -f hostfile ./src 2048 >> plot.txt ;
			
	echo "$execution done"	
done
			




