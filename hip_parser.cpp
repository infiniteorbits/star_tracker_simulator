/*
 * hip_parser.cpp : Hipparcos Parser
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include <stdlib.h>
#include <stdio.h>

#include "Star.hpp"
#define N_PARAMS 27
#define N_RECORDS 120404


// int parse_stcatalog(int mode, Star *records_out[N_RECORDS])
int main(int argc, char const *argv[])
{
	// Temporary/Helper vars
	int n_bright_stars = 0;
	int line_count = 0;
	std::string line;
	std::string filename = "hip2.dat";
	std::string filedir = "./Hipparcos/"; 
	std::ifstream ifs;
	std::string tokens[N_PARAMS];	// TODO: make this the dictionary instead 
									// of the lbl_idx
	
	// Open file
	ifs.open((filedir+filename).c_str());//, ifstream::in);
	if (!ifs) {
	    std::cerr << "Unable to open file" << filename;
	    return 1;
	}

	// Parameter labels and dictionary of indices
	std::string labels[N_PARAMS] = {"HIP","Sn","So","Nc","RArad","DErad","Plx",
		"pmRA","pmDE","e_RArad","e_DErad","e_Plx","e_pmRA","e_pmDE","Ntr",
		"F2","F1","var","ic","Hpmag","e_Hpmag","sHp","VA","B-V","e_B-V","V-I",
		"UW"};
	std::map<std::string, int> lbl_idx;
	for (int i = 0; i < N_PARAMS; ++i)
	{
		lbl_idx[labels[i]] = i;
	}

	// Read stream line by line
	for(line; std::getline(ifs, line); ifs.good()) {   
	    // Make a stream for the line itself
	    std::stringstream iss(line);

	    for (int i = 0; i < N_PARAMS; ++i) {
	    	// and read the whitespace-separated token
	    	iss >> tokens[i];
	    }

	    Star *star = new Star(atoi(tokens[lbl_idx["HIP"]].c_str()),
	    					atof(tokens[lbl_idx["RArad"]].c_str()),
	    					atof(tokens[lbl_idx["DErad"]].c_str()),
	    					atof(tokens[lbl_idx["pmRA"]].c_str()),
	    					atof(tokens[lbl_idx["pmDE"]].c_str()),
	    					atof(tokens[lbl_idx["Plx"]].c_str()),
	    					atof(tokens[lbl_idx["Hpmag"]].c_str()));
	
	// 	Star *star = new Star(atoi(tokens[0].c_str()),	// HIP
	//    					atof(tokens[4].c_str()),	// RArad
	//    					atof(tokens[5].c_str()),	// DErad
	//    					atof(tokens[7].c_str()),	// pmRA
	//    					atof(tokens[8].c_str()),	// pmDE
	//    					atof(tokens[6].c_str()),	// Plx
	//    					atof(tokens[19].c_str()));	// Hpmag
	    if (star->Hpmag <= 6.5)
	    	n_bright_stars++;
	    std::cout << "Star "<< star->id << " has magnitude "<< star->Hpmag<< std::endl;
	}

	std::cout << n_bright_stars << " are brighter than 6.5"<<std::endl;

	ifs.close();
	return 0;
}