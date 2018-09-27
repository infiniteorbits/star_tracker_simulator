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
#include <array>
#include <map>
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>

#include "Star.hpp"
#define N_PARAMS 27


int parse_stcatalog(int mode, Star **records_out);
int sort_stars(Star **records_in, size_t size);

#endif
