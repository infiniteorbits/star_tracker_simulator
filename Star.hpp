/*
 * Star.hpp : Star class
 */
#include <iostream>
#include <fstream>
#define N_PARAMS 27
#define N_RECORDS 120404

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

		double x, y, z; // 3D coordinates
		// Constructor
		Star(int _id, float _RArad, float _DErad, float _pmRA, float _pmDE, 
			float _Plx, float _Hpmag);
		// Star(std::map<string, string> params);
		~Star();
};