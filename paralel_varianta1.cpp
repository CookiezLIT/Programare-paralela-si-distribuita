#include <fstream>
#include <iostream>
#include <mpi.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
ifstream fin;
ifstream fin2;
ofstream fout;

//declaring file names
const string FILE1_NAME = "./data/numar1_mediu.txt";
const string FILE2_NAME = "./data/numar2_mediu.txt";
const string RESULT_FILE_NAME = "./data/rezultat_mediu.txt";
//declaring sizes
const int FILE1_SIZE = 1000;
const int FILE2_SIZE = 1000;
const int RESULT_SIZE = 1001;


void show_result(int c[RESULT_SIZE]){
    /*
     * show the result on the screen and in the output file
     */
    fout.open(RESULT_FILE_NAME);
    if (!fout.is_open()){
        throw exception();
    }
    for (int i =0; i < RESULT_SIZE; i++){
//        cout << c[i] << " ";
        fout << c[i] << " ";
    }
    fout.close();
}

int a1[FILE1_SIZE], b1[FILE2_SIZE];

int main(){

    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // processor zero does not do any additions
    int buffer_size = RESULT_SIZE / (world_size-1);
    MPI_Status status;
    // processor 0 reads the n/p numbers and sends them to the other processors
    if (world_rank == 0){

//        int* a = new int(RESULT_SIZE);
//        int* b = new int(RESULT_SIZE);
        int* c = new int(RESULT_SIZE);

        int id_current_process = 1;
        cout << "PROCESSOR 0 SENT THE DATA" << endl;
        fin.open(FILE1_NAME, ios::in | ios::binary);
        fin2.open(FILE2_NAME, ios::in | ios::binary);
        auto start = chrono::high_resolution_clock::now();
        if (fin.is_open() and fin2.is_open()){
            for (int i =0; i<max(FILE1_SIZE,FILE2_SIZE);i++){
                if (i < FILE1_SIZE){
                    fin >> a1[i];
                }
                if (i < FILE2_SIZE) {
                    fin2 >> b1[i];
                }
                // we read n/p numbers, send them to the process
                if (i % buffer_size == 0 and i != 0){
                    MPI_Send(a1+(id_current_process-1) * buffer_size,buffer_size,MPI_INT,id_current_process,0,MPI_COMM_WORLD);
                    MPI_Send(b1+(id_current_process-1) * buffer_size,buffer_size,MPI_INT,id_current_process,0,MPI_COMM_WORLD);
                    id_current_process += 1;
                }
            }
            // if data did not divide to the number of processors, clear the buffer for the last process
            if (id_current_process == world_size - 1){
                MPI_Send(a1+(id_current_process-1) * buffer_size,buffer_size,MPI_INT,id_current_process,0,MPI_COMM_WORLD);
                MPI_Send(b1+(id_current_process-1) * buffer_size,buffer_size,MPI_INT,id_current_process,0,MPI_COMM_WORLD);
                id_current_process += 1;
            }
            fin.close();
            fin2.close();
        }
        else{
            throw std::exception();
        }

        cout << "PROCESSOR 0 SENT THE DATA" << endl;
        // Recieve from the other processes and print the array
        for(int i=1;i<world_size;i++){
            MPI_Recv(c+(i-1)*buffer_size,buffer_size,MPI_INT,i,2,MPI_COMM_WORLD,&status);
        }
        // Recieve carry from the last processor
        int aux = 0;
        MPI_Recv(&aux,1,MPI_INT,world_size-1,1,MPI_COMM_WORLD,&status);
        c[RESULT_SIZE-1] += aux;
        auto end = chrono::high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << duration.count() << endl;
//        for (int i=0; i<RESULT_SIZE;i++)
//            cout << c[i] << " ";
        show_result(c);
    }
    else if (world_rank < world_size){
        int a[buffer_size];
        int b[buffer_size];
        int c[buffer_size];
//        int* b = new int(buffer_size);
//        int* c = new int(buffer_size);
        int carry = 0;
        // processor 1 does not recieve any carry from the previous processor; tag is 1

        MPI_Recv(a,buffer_size,MPI_INT,0,0,MPI_COMM_WORLD, &status);
        MPI_Recv(b,buffer_size,MPI_INT,0,0,MPI_COMM_WORLD, &status);
        if (world_rank != 1){
            MPI_Recv(&carry, 1, MPI_INT, world_rank-1,1,MPI_COMM_WORLD, &status);
        }
        //add the numbers
        for (int i=0;i<buffer_size;i++){
            c[i] = (a[i] + b[i] + carry) % 10;
            if (a[i] + b[i] + carry >= 10 ){
                carry = 1;
            }
            else carry = 0;
        }

        //last processor sends the carry to the first processor
        if (world_rank != world_size - 1){
            MPI_Send(&carry,1,MPI_INT,world_rank + 1,1,MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(&carry,1,MPI_INT,0,1,MPI_COMM_WORLD);
        }
        //send the result to processor one with the tag 2
        MPI_Send(c,buffer_size,MPI_INT,0,2,MPI_COMM_WORLD);
        cout << "PROCESSOR " << world_rank << " TERMINATED" << endl;
    }
    MPI_Finalize();
    return 0;
}
