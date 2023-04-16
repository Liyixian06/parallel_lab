#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

void trans_matrix(string str, int row, int col, int** array){
	stringstream ss(str);
	int num;
	while(ss>>num)
		array[row][col-num-1]=1;
}

int find_pivot(int* array, int col){
	for(int i=0;i<col;i++){
		if(array[i]==1)
			return i;
	}
	return -1;
}

int main(){
	//create elimination matrix
	int col=130, eli_row=22, to_be_eli_row=8;
	int** R = new int*[eli_row]; //消元子
	for(int i=0;i<eli_row;i++)
		R[i]=new int[col];
	for(int i=0;i<eli_row;i++)
		for(int j=0;j<col;j++)
			R[i][j]=0;
	int** E = new int*[to_be_eli_row]; //被消元行
	for(int i=0;i<to_be_eli_row;i++)
		E[i]=new int[col];
	for(int i=0;i<to_be_eli_row;i++)
		for(int j=0;j<col;j++)
			E[i][j]=0;
	
	//input data
	ifstream eli, to_be_eli;
	ofstream result;
	eli.open("1.txt", ios::in);
	vector<string> eli_vec;
	string eli_line;
	while(getline(eli, eli_line))
		eli_vec.push_back(eli_line);
	to_be_eli.open("2.txt", ios::in);
	vector<string> to_be_eli_vec;
	string to_be_eli_line;
	while(getline(to_be_eli, to_be_eli_line))
		to_be_eli_vec.push_back(to_be_eli_line);

	//transform to matrix
	for(int i=0;i<eli_vec.size();i++)
		trans_matrix(eli_vec[i],i,col,R);
	for(int i=0;i<to_be_eli_vec.size();i++)
		trans_matrix(to_be_eli_vec[i],i,col,E);
	
	eli.close();
	to_be_eli.close();
	
	/*
	for(int i=0;i<eli_row;i++){
		for(int j=0;j<col;j++)
			cout<<R[i][j]<<" ";
		cout<<endl;
	}
	for(int i=0;i<to_be_eli_row;i++){
		for(int j=0;j<col;j++)
			cout<<E[i][j]<<" ";
		cout<<endl;
	}
	*/
	
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL);
	for(int i=0;i<to_be_eli_row;i++){
		while(find_pivot(E[i], col)!=-1){
			int exist = find_pivot(E[i],col);
			if(find_pivot(R[exist],col)!=-1){
				for(int j=0;j<col;j++)
					E[i][j]=E[i][j]^R[exist][j];
			}
			else{
				for(int j=0;j<col;j++)
					R[exist][j]=E[i][j];
				break;
			}
		}
	}
	gettimeofday(&end, NULL);
	float time = (end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	
	return 0;
}
