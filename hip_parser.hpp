/*
 * hip_parser.hpp: Hipparcos catalog parser class
 *
 */
#ifndef HIP_PARSER_HPP
#define HIP_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include <stdlib.h>
#include <stdio.h>

#include "Star.hpp"
#define N_PARAMS 27
#define N_RECORDS 117955


int parse_stcatalog(int mode, Star **records_out);

#endif
