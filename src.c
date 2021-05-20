#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Global variables.
int rank, size, n, no_of_groups = 0;
MPI_Comm groupcomm;
MPI_Comm masters_comm;
MPI_Comm local;
MPI_Comm cross;
double comm_creation_time;
int group;


void new_bcast(double data[],int root_rank)
{
	double stime, etime, maxtime;
        // Default bcast.
        stime = MPI_Wtime();
   	for(int i=0;i<5;i++){
           MPI_Bcast(data, n, MPI_DOUBLE,root_rank,MPI_COMM_WORLD);
       	}
    	etime = MPI_Wtime() - stime;
   	MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, size-1, MPI_COMM_WORLD); //obtaining the slowest process time.
    	if(rank==size-1) printf("%lf \n", maxtime/5);
	
    stime=MPI_Wtime();
	int lrank;
    int mrank =0;
    	MPI_Comm_rank(local,&lrank);
    if(size%32==0 && root_rank!=0){
        int gr = root_rank/8 +1; 
        if(rank == root_rank){
            MPI_Send(data,n,MPI_DOUBLE,0,999,local);
        }
        if(group==gr){
            if(lrank==0){
                MPI_Recv(data,n,MPI_DOUBLE,MPI_ANY_SOURCE,999,local,MPI_STATUS_IGNORE);
                MPI_Comm_rank(cross,&mrank);
            }
        }
    }
       
	for(int i=0;i<5;i++)
	{
		if(lrank==0)
		{
			MPI_Bcast(data,n,MPI_DOUBLE,0,cross);
		}
		MPI_Bcast(data,n,MPI_DOUBLE,0,local);
	}
	etime = MPI_Wtime() - stime + comm_creation_time;
    	MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, 0, local);
    	if(rank==0) printf("%lf \n",  (maxtime/5)); 
	
}	

void new_opti_reduce(double data[],double maxdata[], int root_rank)
{
	
	MPI_Reduce(data,maxdata,n,MPI_DOUBLE,MPI_MAX,0,local);
	int lrank;
    	MPI_Comm_rank(local,&lrank);
    int mrank =0;
    if(size%32==0 && root_rank!=0){
        int gr = root_rank/8 +1; 
        if(group==gr){
            if(lrank==0){
                MPI_Comm_rank(cross,&mrank);
            }
        }
    }
	if(lrank==0)
	{
		MPI_Reduce(data,maxdata,n,MPI_DOUBLE,MPI_MAX,mrank,cross);
	}
    if(size%32==0 && root_rank!=0){
        int gr = root_rank/8 +1; 
        if(rank == root_rank){
            MPI_Recv(maxdata,n,MPI_DOUBLE,MPI_ANY_SOURCE,999,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        if(group==gr){
            if(lrank==0){
                MPI_Send(maxdata,n,MPI_DOUBLE,root_rank,999,MPI_COMM_WORLD);
            }
        }
    }
}
void new_reduce(double data[],double maxdata[],int root_rank)
{
    double stime, etime, maxtime;
    // Default reduce.
    stime = MPI_Wtime();
    for(int i=0;i<5;i++){
        MPI_Reduce(data, maxdata,n, MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    }
    etime = MPI_Wtime() - stime ;
    MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //obtaining the slowest process time.
    if(rank==0) printf("%lf\n", maxtime/5);

   //optimized reduce 
    stime = MPI_Wtime();
    for(int i=0;i<5;i++){
        new_opti_reduce(data,maxdata,root_rank);   
    }
    etime = MPI_Wtime() - stime + comm_creation_time;
    MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //obtaining the slowest process time.
    if(rank==0) printf("%lf\n", (maxtime/5)); 
	
}

void new_opti_gather(double data[],int root_rank)
{
	int group_size;
	MPI_Comm_size(local,&group_size);
	double*recvdata=(double*)malloc(n*group_size*sizeof(double));
	MPI_Gather(data,n,MPI_DOUBLE,recvdata,n,MPI_DOUBLE,0,local);
	
	int lrank;
    	MPI_Comm_rank(local,&lrank);
    int mrank =0;
    if(size%32==0 && root_rank!=0){
        int gr = root_rank/8 +1; 
        if(group==gr){
            if(lrank==0){
                MPI_Comm_rank(cross,&mrank);
            }
        }
    }
    double*Recv=(double*)malloc(n*size*sizeof(double));
	if(lrank==0)
	{
		MPI_Gather(recvdata,n*group_size,MPI_DOUBLE,Recv,n*group_size,MPI_DOUBLE,mrank,cross);
	}
    if(size%32==0 && root_rank!=0){
        int gr = root_rank/8 +1; 
        if(rank == root_rank){
            MPI_Recv(Recv,n*size,MPI_DOUBLE,MPI_ANY_SOURCE,999,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        if(group==gr){
            if(lrank==0){
                MPI_Send(Recv,n*size,MPI_DOUBLE,root_rank,999,MPI_COMM_WORLD);
            }
        }
    }

	return ;
}
	
	
void new_gather(double data[],int root_rank)
{
    double stime, etime, maxtime;
    double * recvdata=(double*)malloc(n*size*sizeof(double));
    // Default gather.
    stime = MPI_Wtime();
    for(int i=0;i<5;i++){
        MPI_Gather(data,n,MPI_DOUBLE,recvdata,n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }
    etime = MPI_Wtime() - stime;
    MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //obtaining the slowest process time.
    if(rank==0) printf("%lf\n", maxtime/5);  // avg time - maxtime/5
  
    stime = MPI_Wtime();
    for(int i=0;i<5;i++){
        new_opti_gather(data,root_rank);
    }
    etime = MPI_Wtime() - stime + comm_creation_time;
    MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //obtaining the slowest process time.
    if(rank==0) printf("%lf\n", (maxtime/5));  // avg time - maxtime/5
	
}
    
   
// Function to implement optimized algo for alltoallv.
void opti_alltoallv(double data[],int D,int masters[]){

	
}

// Function to compare the performance of default and optimized alltoallv.
void alltoallv(double data[],int D,int masters[]){
    double stime, etime, maxtime;
    int arrSize=(D*128)/size;
    double message[arrSize];
    for(int i=rank*arrSize,j=0;i<((rank+1)*arrSize);i++,j++)
    {
	message[j]=data[i];
    }
    int countArray1[size], displArray1[size],displArray2[size],countArray2[size];
    int displ=0;
    for(int i=0;i<size;i++)
    {
	countArray1[i]=(arrSize/size);
	displArray1[i]=displ;
	displ+=countArray1[i];
	countArray2[i]=countArray1[i];
	displArray2[i]=displArray1[i];
	
    }
    double recvMessage[arrSize];  
    // Default alltoallv.
    stime = MPI_Wtime();
    for(int i=0;i<5;i++){
        MPI_Alltoallv(message,countArray1, displArray1,MPI_DOUBLE, recvMessage,countArray2, displArray2, MPI_DOUBLE,MPI_COMM_WORLD);
    }
    etime = MPI_Wtime() - stime;
    MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, size-1, MPI_COMM_WORLD); //obtaining the slowest process time.
    if(rank==size-1) printf("Default gather time: %lf\n", maxtime/5);  // avg time - maxtime/5
    
    // Optimized alltoallv.
    stime = MPI_Wtime();
    for(int i=0;i<5;i++){
        opti_alltoallv(data,D,masters);
    }
    etime = MPI_Wtime() - stime;
    MPI_Reduce(&etime, &maxtime,1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //obtaining the slowest process time.
    if(rank==0) printf("Optimized gather time: %lf\n", maxtime/5);  // avg time - maxtime/5

}

// Global variables
char arry1[15][10] = {"csews2","csews3","csews4","csews5","csews6","csews7","csews8","csews9","csews10","csews11","csews12","csews14","csews15","csews16","csews31"};
char arry2[17][10] = {"csews13","csews17","csews18","csews19","csews20","csews21","csews22","csews23","csews24","csews25","csews26","csews27","csews28","csews29","csews30","csews32"};
char arry3[14][10] = {"csews33","csews34","csews35","csews36","csews37","csews38","csews39","csews40","csews41","csews42","csews43","csews44","csews46"};
char arry4[15][10] = {"csews45","csews47","csews48","csews49","csews50","csews51","csews52","csews53","csews54","csews56","csews58","csews59","csews60","csews61"};
char arry5[18][10] = {"csews62","csews63","csews64","csews65","csews66","csews67","csews68","csews69","csews70","csews71","csews72","csews73","csews74","csews75","csews76","csews77","csews78"};
char arry6[15][10] = {"csews79","csews80","csews81","csews82","csews83","csews84","csews85","csews86","csews87","csews88","csews89","csews90","csews91","csews92"};
int f1=0,f2=0,f3=0,f4=0,f5=0,f6=0;

void groupcounter(char * ptr){
    for(int i=0; i<15 && f1==0; i++){
        if(!strcmp(arry1[i],ptr)){
            f1 =1;
            no_of_groups++;
            break;
        }
    }
    for(int i=0; i<17 && f2==0; i++){
        if(!strcmp(arry2[i],ptr)){
            f2 =1;
            no_of_groups++;
            break;
        }
    }
    for(int i=0; i<14 && f3==0; i++){
        if(!strcmp(arry3[i],ptr)){
            f3 =1;
            no_of_groups++;
            break;
        }
    }
    for(int i=0; i<15 && f4==0; i++){
        if(!strcmp(arry4[i],ptr)){
            f4 =1;
            no_of_groups++;
            break;
        }
    }
    for(int i=0; i<18 && f5==0; i++){
        if(!strcmp(arry5[i],ptr)){
            f5 =1;
            no_of_groups++;
            break;
        }
    }
    for(int i=0; i<15 && f6==0; i++){
        if(!strcmp(arry6[i],ptr)){
            f6 =1;
            no_of_groups++;
            break;
        }
    }
}

int main(int argc, char*argv[]){
    n = atoi(argv[1]) *128;   // number of doubles that are needed to be createed per rank.
    double data[n];
    int D=atoi(argv[1]);
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Reading the hostfile
    char c[1000];
    FILE *fptr;
    if ((fptr = fopen("hostfile", "r")) == NULL) {
        printf("Error! opening hostfile");
        exit(1);
    }

    char *line = NULL;
    size_t leng = 0;
    char *ptr;
     while(getline(&line, &leng, fptr) != -1) {
        char delim[] = ":";
        ptr = strtok(line, delim);
        // printf("%s\n", ptr);
        groupcounter(ptr); // counting number of groups 
     }
    fclose(fptr);
    
// Identifying the group number to which rank belongs to.
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(proc_name, &len);
    // printf("processor name: %s\n",proc_name);

    for(int i=0; i<15; i++){
        if(!strcmp(arry1[i],proc_name)){
            group = 1;
            break;
        }
    }
    for(int i=0; i<17; i++){
        if(!strcmp(arry2[i],proc_name)){
            group = 2;
            break;
        }
    }
    for(int i=0; i<14; i++){
        if(!strcmp(arry3[i],proc_name)){
            group = 3;
            break;
        }
    }
    for(int i=0; i<15; i++){
        if(!strcmp(arry4[i],proc_name)){
            group = 4;
            break;
        }
    }
    for(int i=0; i<18; i++){
        if(!strcmp(arry5[i],proc_name)){
            group = 5;
            break;
        }
    }
    for(int i=0; i<15; i++){
        if(!strcmp(arry6[i],proc_name)){
            group = 6;
            break;
        }
    } 
    double stime=MPI_Wtime();
    MPI_Comm_split(MPI_COMM_WORLD,group,0,&local);

    int lrank;
    MPI_Comm_rank(local,&lrank);
    if(lrank!=0)
    	MPI_Comm_split(MPI_COMM_WORLD,MPI_UNDEFINED,0,&cross);
    else
	MPI_Comm_split(MPI_COMM_WORLD,lrank,0,&cross);
    double etime=MPI_Wtime()-stime;
    comm_creation_time = etime;
    // MPI_Reduce(&etime, &comm_creation_time,1, MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD); //obtaining the slowest process time.
    // printf("comm: %lf\n",comm_creation_time);
    

    for(int i =0; i<n;i++) data[i] = rand();  // Initializing the data with random values.
    int root_rank = 3;
    new_bcast(data,root_rank);
    double maxdata[n];
    new_reduce(data,maxdata,root_rank);
    new_gather(data,root_rank);
   // bcast(data,masters);
   // reduce(data,masters);
    //gather(data,masters);
     //alltoallv(data,D,masters);

    MPI_Finalize();
}
