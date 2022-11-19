#include <fstream>
#include <string>
#include <iostream>
#include <utility> // for the swap function
#include <cstdlib> // for random
#include <time.h> // for setting the random seed
#include <chrono>
using namespace std;
using namespace std::chrono;
ifstream fin;
ofstream fout;

//declaring file names
const string FILE1_NAME = "./data/numar1_mare.txt";
const string FILE2_NAME = "./data/numar2_mare.txt";
const string RESULT_FILE_NAME = "./data/rezultat_mare.txt";
//declaring sizes
const int FILE1_SIZE = 100;
const int FILE2_SIZE = 100000;
const int RESULT_SIZE = 100001;

void generate_random_numbers(string file, int size){
    /*
     * generate :size: random numbers and write them into :file:
     */
    srand(time(0));
    fout.open(file);
    if (!fout.is_open()){
        throw exception();
    }
    for (int i=0; i<size; i++){
        fout << rand() % 10 << " ";
    }
}

void read_numbers(string file1, string file2, int a[FILE1_SIZE], int b[FILE2_SIZE]){

    fin.open(file1);
    if (fin.is_open()){
        for (int i =0; i<FILE1_SIZE;i++){
            fin >> a[i];
        }
        fin.close();
    }
    else{
        throw std::exception();
    }
    fin.open(file2);
    if (fin.is_open()){
        for (int i =0; i<FILE2_SIZE;i++){
            fin >> b[i];
        }
        fin.close();
    }
    else{
        throw std::exception();
    }
}

void add_numbers(int a[FILE1_SIZE], int b[FILE2_SIZE], int c[RESULT_SIZE])
{
    int rest = 0;
    //always keep smaller array the second:
    if (FILE1_SIZE < FILE2_SIZE){
        swap(a,b);
    }
    for (int i=0;i<RESULT_SIZE;i++){
        if (i < FILE2_SIZE){
            c[i] = (a[i] + b[i] + rest) % 10;
            if (a[i] + b[i] + rest >= 10 ){
                rest = 1;
            }
            else rest=0;
        }
        else if (i < FILE1_SIZE){ // one array is done, the sum from now is just copying the values
            c[i] = a[i] + rest;
            rest = 0;
        }
        else{ // last digit
            c[i] = rest;
        }

    }
}

void show_result(int c[RESULT_SIZE]){
    /*
     * show the result on the screen and in the output file
     */
    fout.open(RESULT_FILE_NAME);
    if (!fout.is_open()){
        throw exception();
    }
    for (int i =0; i < RESULT_SIZE; i++){
        //cout << c[i] << " ";
        fout << c[i] << " ";
    }
    fout.close();
}
int a[FILE1_SIZE], b[FILE2_SIZE], c[RESULT_SIZE];

int main(){
    read_numbers(FILE1_NAME,FILE2_NAME,a,b);
    auto start = chrono::high_resolution_clock::now();
    add_numbers(a,b,c);
    auto end = chrono::high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << duration.count() << endl;
    show_result(c);
    //generate_random_numbers("./data/numar2_mare.txt",100000);
    return 0;
}