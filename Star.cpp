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
	pmRA = _pmRA;		// in mas/yr
	pmDE = _pmDE;		// in mas/yr
	Plx = _Plx;		// Parallax in mas
	Hpmag = _Hpmag;
}