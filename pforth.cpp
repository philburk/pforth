// pforth.cpp 
// Created by Robin Rowe 2020-05-01
// License Public Domain

#include <iostream>
using namespace std;

void Usage()
{	cout << "Usage: pforth " << endl;
}

enum
{	ok,
	invalid_args

};

int main(int argc,char* argv[])
{	cout << "pforth starting..." << endl;
	if(argc < 1)
	{	Usage();
		return invalid_args;
	}

	cout << "pforth done!" << endl;
	return ok;
}
