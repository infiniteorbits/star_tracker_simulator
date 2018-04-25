/*
 * star_tracker_input.cpp: 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>

#include <stdlib.h>
#include <stdio.h>

#include "hip_parser.hpp"

#define N_PARAMS 27
#define N_RECORDS 117955


// int parse_stcatalog(int mode, Star *records_out[N_RECORDS])
int main(int argc, char const *argv[])
{
	Star** records = new Star*[N_RECORDS];
	for (int i = 0; i < N_RECORDS; ++i)
	{
		records[i] = new Star();
	}

	// parse returns true if there is an error
	if (parse_stcatalog(0, records)) {
		std::cerr << "parse_stcatalog failed" << std::endl;
		std::exit(1);
	}

	std::cout << "Catalog parsed" << std::endl;

	int bright_stars = 0;
	for (int i = 0; i < N_RECORDS; ++i)
	{
		if (records[i]->Hpmag <= 6)
			bright_stars++;
	}

	std::cout << "there are " << bright_stars << " bright_stars"<< std::endl;

	// remember to free those stars (TODO: clean the heap on errors)
	for (int i = 0; i < N_RECORDS; ++i)
	{
		delete records[i];
	}
	delete [] records;
	
	return 0;
}