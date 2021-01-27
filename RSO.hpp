/*
 * RSO.hpp : a sub-class of Star to represent a moving RSO
 */
#ifndef RSO_HPP
#define RSO_HPP

#include "Star.hpp"

class RSO: public Star 
{
    public:
        double RA_speed = 0.01; // in rad/sec
        double DE_speed = 0.01;
        double fake_rho = 1;
		RSO(int _id, float _RArad, float _DErad, float _pmRA, float _pmDE, 
		    float _Plx, float _Hpmag) :
		    Star(_id, _RArad, _DErad, _pmRA, _pmDE, _Plx, _Hpmag) 
        {               
        
        }
		
        RSO(int _id, float _RArad, float _DErad, float _pmRA, float _pmDE, 
		    float _Plx, float _Hpmag, double _RA_speed, double _DE_speed) :
		    Star(_id, _RArad, _DErad, _pmRA, _pmDE, _Plx, _Hpmag), 
            RA_speed(_RA_speed), DE_speed(_DE_speed)
        {
                
        }
        
        RSO() : Star()
        {
        
        }

        inline void setRA(float _RArad)
        {
            RArad = _RArad;
            calculate_cartesian(RArad, DErad, fake_rho);
        }

        inline void setDE(float _DErad)
        {
            DErad = _DErad;
            calculate_cartesian(RArad, DErad, fake_rho);
        }
};
#endif
