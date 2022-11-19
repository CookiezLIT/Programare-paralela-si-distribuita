#include <iostream>
#include <fstream>
#include <mpi.h>
#include <algorithm> // for max
using namespace std;

ifstream fin;
ofstream fout;

//declaring file names
const string FILE1_NAME = "./data/numar1_mediu.txt";
const string FILE2_NAME = "./data/numar2_mediu.txt";
const string RESULT_FILE_NAME = "./data/rezultat_mare.txt";

//declaring sizes
const int FILE1_SIZE = 1000;
const int FILE2_SIZE = 1000;
const int RESULT_SIZE = 1001;

int a1[2*RESULT_SIZE], b1[2*RESULT_SIZE];

int main(){
    MPI_Init(NULL,NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    MPI_Status mpiStatus;

    int total_size;
    int buffer_size;
    int zeros_to_add;

    //processor 0 reads the numbers, adds 0 if the size of the numbers differ, and sends the result to the other processes
    if (world_rank == 0){
        fin.open(FILE1_NAME);
        for(int i = 0; i < FILE1_SIZE; i++)
            fin >> a1[i];
        fin.close();
        fin.clear();
        fin.open(FILE2_NAME);
        for (int i = 0; i < FILE2_SIZE; i++)
            fin >> b1[i];
        fin.close();
        // init total size with the length of the bigger number
        total_size = max(FILE1_SIZE,FILE2_SIZE);

        //calculate the total_size to be divided by the number of processes
        zeros_to_add = 0;
        if (total_size % world_size != 0){
            zeros_to_add = world_size - (total_size % world_size);
        }
        total_size = total_size + zeros_to_add;

        //add zeros to the numbers
        for(int i=FILE1_SIZE;i<total_size;i++)
            a1[i] = 0;
        for(int i=FILE2_SIZE;i<total_size;i++)
            b1[i] = 0;
        // send the total_size to the other processes with tag 0
        for(int i=1;i<world_size;i++)
            MPI_Send(&total_size,1,MPI_INT,i,0,MPI_COMM_WORLD);
        buffer_size = total_size / world_size;
        // send the total_size to the other processes with tag 1
        for(int i=1;i<world_size;i++)
            MPI_Send(&buffer_size,1,MPI_INT,i,1,MPI_COMM_WORLD);
    }
    if (world_rank != 0){
        //recieve the buffer_size and the total_size

        MPI_Recv(&total_size,1,MPI_INT,0,0,MPI_COMM_WORLD,&mpiStatus);
        MPI_Recv(&buffer_size,1,MPI_INT,0,1,MPI_COMM_WORLD,&mpiStatus);
    }

    int* a;
    int* b;
    int* c;
    int* c2;
    a = new int(buffer_size);
    b = new int(buffer_size);
    c = new int(total_size);
    c2 = new int(total_size);

    MPI_Scatter(a1,buffer_size,MPI_INT,a,buffer_size,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(b1,buffer_size,MPI_INT,b,buffer_size,MPI_INT,0,MPI_COMM_WORLD);

    int carry = 0;
    if (world_rank != 0) {
        MPI_Recv(&carry, 1, MPI_INT,world_rank-1,3,MPI_COMM_WORLD,&mpiStatus);
    }
    for (int i=0;i<buffer_size;i++){
        c2[i] = (a[i] + b[i] + carry) % 10;
        carry = 0;
        if (a[i] + b[i] + carry > 9){
            carry = 1;
        }
        //cout << c2[i] << " ";
    }
    if (world_rank != world_size - 1){
        MPI_Send(&carry,1,MPI_INT,world_rank+1,3,MPI_COMM_WORLD);
    }
    else{
        MPI_Send(&carry,1,MPI_INT,0,3,MPI_COMM_WORLD);
    }
    cout << endl;
    MPI_Gather(c2,buffer_size,MPI_INT,c,buffer_size,MPI_INT,0,MPI_COMM_WORLD);


    if (world_rank == 0){
        MPI_Recv(&carry,1,MPI_INT,world_size-1,3,MPI_COMM_WORLD,&mpiStatus);
        c[total_size-zeros_to_add] += carry;
        cout << endl << total_size-zeros_to_add << endl;
        for (int i=0;i<total_size;i++)
            cout << c[i] << " ";
    }
    MPI_Finalize();
}