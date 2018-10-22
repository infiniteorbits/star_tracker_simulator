/* 
 * Star.hpp : Star class
 */
#ifndef STAR_HPP
#define STAR_HPP

#include <iostream>
#include <fstream>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>

#define N_PARAMS 27
#define DEG2AS 3600
#define AS2MAS 1000

const double PI = 3.141592653589793238463;

class Star {
	public:
		// static string labels[N_PARAMS] = {"HIP","Sn","So","Nc","RArad","DErad",
		// 	"Plx","pmRA","pmDE","e_RArad","e_DErad","e_Plx","e_pmRA","e_pmDE",
		// 	"Ntr","F2","F1","var","ic","Hpmag","e_Hpmag","sHp","VA","B-V",
		// 	"e_B-V","V-I","UW"};
		
		// needed vars
		int id;// 
		float RArad;	// ICRS
		float DErad;	// ICRS
		float pmRA;		// in mas/yr
		float pmDE;		// in mas/yr
		float Plx;		// Parallax in mas
		float Hpmag;	// Hipparcos magnitude

		double x, y, z; // 3D cartesian coordinates (unit sphere)
		double distance; // real distance calculated for stars with Plx
		// Constructor
		Star(int _id, float _RArad, float _DErad, float _pmRA, float _pmDE, 
			float _Plx, float _Hpmag);
		// Null constructor
		Star();
		// Star(std::map<string, string> params);
		~Star();

        // operators
        friend bool operator> (const Star &s1, const Star &s2);
        friend bool operator>= (const Star &s1, const Star &s2);
        friend bool operator== (const Star &s1, const Star &s2);
        friend bool operator<= (const Star &s1, const Star &s2);
        friend bool operator< (const Star &s1, const Star &s2);


		// calculate cartesian coordinates and save as members
		int calculate_cartesian(float RA, float DE, float rho);

		/*** Helper functions ***/
		// calculate distance from RA, DE, Plx
		static double calculate_distance(float parallax);
		static inline double asec2deg(double x) { return x/DEG2AS; }
		static inline double deg2rad(double x) { return x*PI/180; }
		static inline double mas2rad(double x)
		{ return deg2rad(asec2deg(x/AS2MAS)); }
};
#endif
