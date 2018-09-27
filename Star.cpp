/*
 * Star.hpp : Star class implementation
 */
#include "Star.hpp"

Star::Star(int _id, float _RArad, float _DErad, float _pmRA, float _pmDE, 
	float _Plx, float _Hpmag)
{
	id = _id;// 
	RArad = _RArad;	// ICRS
	DErad = _DErad;	// ICRS
	pmRA = _pmRA;	// in mas/yr
	pmDE = _pmDE;	// in mas/yr
	Plx = _Plx;		// Parallax in mas
	Hpmag = _Hpmag;

	float fake_rho = 1; // fake for now (TODO)
	if(calculate_cartesian(RArad, DErad, fake_rho)) {
		std::cerr<<"calculate_cartesian failed on input"<<RArad<<", "<<DErad<<
		", "<<fake_rho;
		exit(1);
	} 

	distance = Star::calculate_distance(Plx);
}

Star::Star()
{
	id = 0;// 
	RArad = 0;	// ICRS
	DErad = 0;	// ICRS
	pmRA = 0;	// in mas/yr
	pmDE = 0;	// in mas/yr
	Plx = 0;	// Parallax in mas
	Hpmag = 0;

	float fake_rho = 1; // fake for now (TODO)
	if(calculate_cartesian(RArad, DErad, fake_rho)) {
		std::cerr<<"calculate_cartesian failed on input"<<RArad<<", "<<DErad<<
		", "<<fake_rho;
		exit(1);
	} 
	distance = Star::calculate_distance(Plx);
}

int Star::calculate_cartesian(float ra, float de, float rho)
{
	//x = rho*cos(de)*sin(ra);
	//y = rho*sin(de);
	//z = rho*cos(de)*cos(ra);
	x = rho*cos(de)*cos(ra);
	y = rho*cos(de)*sin(ra);
	z = rho*sin(de);

	return 0;
}
// operators
bool operator> (const Star &s1, const Star &s2)
{
    return s1.Hpmag < s2.Hpmag;
}
bool operator>= (const Star &s1, const Star &s2)
{
    return s1.Hpmag <= s2.Hpmag;
}
bool operator== (const Star &s1, const Star &s2)
{
    return s1.Hpmag == s2.Hpmag;
}
bool operator<= (const Star &s1, const Star &s2)
{
    return s1.Hpmag >= s2.Hpmag;
}
bool operator< (const Star &s1, const Star &s2)
{
    return s1.Hpmag > s2.Hpmag;
}


// calculate distance Plx in AU
double Star::calculate_distance(float parallax) 
{
	// L = R/tan(theta) where L is the distance to the star, R is the distance
	// to the sun and theta is half the apparent angular shift = parallax angle
	double theta = Star::mas2rad((double) parallax); // in radians
	return (1/tan(theta)); // in AU
}

Star::~Star()
{
	
}
