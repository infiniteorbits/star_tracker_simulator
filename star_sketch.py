#!/usr/bin/python3

# python sketch of the ST Sim algo
import math
import random
import numpy as np 
import cv2
import os, sys

# DEBUG
global ins_fov_ctr
global n_stars
ins_fov_ctr = 0
n_stars = 0

##################### Helper functions #########################################

# limit check for naive implementation of the projection
def between(x, lower_lim, upper_lim):
    if (x<lower_lim or x>upper_lim):
        return False    
    return True

def inside(x, center, fov):
    return between(x, (center-(fov/2)), (center+(fov/2)))

def inside_RA(x, center, fov):
    if ((center-fov/2)<0):
        return (between(x, (center-(fov/2)+2*math.pi), 2*math.pi)
        or between(x, 0, (center+(fov/2))))
    return between(x, (center-(fov/2)), (center+(fov/2)))

def inside_declination(x, center, fov):
    return inside(x, center, fov)

def random_angle_in_deg():
    return random.uniform(0, 360)

def random_angle_in_rad():
    return random.uniform(0, 2*math.pi)

################################################################################


class Star:
    """honestly not much of a reason rn but might be useful later"""
    def __init__(self, star_alpha, star_delta, Vmag):
        # DEBUG
        global n_stars
        n_stars += 1
        # super(Star, self).__init__()
        self.Vmag = Vmag
        self.alpha = star_alpha
        self.delta = star_delta


def get_projection_coordinates(fov_alpha, fov_delta, center_alpha, center_delta, 
    st):
    # all params in radians
    # rotate celestial sphere
    star_rel_alpha = st.alpha-center_alpha
    star_rel_delta = st.delta-center_delta
    # normalize after transformation
    if (star_rel_alpha >= 2*math.pi): # [0, 2pi)
        star_rel_alpha -= 2*math.pi
    elif (star_rel_alpha < 0):
        star_rel_alpha += 2*math.pi
        
    if (star_rel_delta < -math.pi/2):
        # reflect
        star_rel_delta = (star_rel_delta + math.pi)*(-1) 
        star_rel_alpha = star_rel_alpha+math.pi if star_rel_alpha<math.pi \
            else star_rel_alpha-math.pi
    elif (star_rel_delta >= math.pi/2):
        # reflect
        star_rel_delta = (star_rel_delta - math.pi)*(-1) 
        star_rel_alpha = star_rel_alpha+math.pi if star_rel_alpha<math.pi \
            else star_rel_alpha-math.pi            

    # check whether the star is inside the field of view
    if (not(inside_RA(star_rel_alpha, 0, fov_alpha)) or
        not(inside_declination(star_rel_delta, 0, fov_delta))):
        return (-1, -1) # star is outside the field of view
        pass

    # DEBUG
    global ins_fov_ctr
    ins_fov_ctr += 1

    # TODO what's the solution with divide by zero when fov is 180 deg (see above)
    x = (math.tan(star_rel_alpha)/(2*math.tan(fov_alpha/2))) + 0.5
    y = (math.tan(star_rel_delta)/(2*math.tan(fov_delta/2))) + 0.5
    # print(x,y)
    return (x, y)


# looking for [x,y] where x, y are floats b/w 0 and 1 indicating their position
# unto the image plane



##################### Main  ####################################################

def main(alpha_deg = 0, delta_deg = 0):
    
    # given angles on the celestial sphere
    fov_alpha = math.radians(90) # i'll call this the x, relative span of afov, right ascension
    fov_delta = math.radians(45) # i'll call this the y, relative span of afov, declination
    center_alpha = math.radians(alpha_deg) # from [0 to 360) deg
    center_delta = math.radians(delta_deg) # from [-90 to 90) deg # from [0 to 180) deg
    
    # Create a black image
    xres = 2048
    yres = 2048/2
    img = np.zeros((yres, xres, 3), np.uint8)
    white = (255,255,255)

    stars = []
    # # Create random stars
    # for x in range(-90,90): # declination is in [-pi/2,pi/2]
    #     for y in range(0,360):
    #         stars.append(Star(random_angle_in_rad(), random_angle_in_rad(), 
    #             random.randrange(1, 7)))
    #         # print(str(stars[y+(x*360)].alpha)+', '+str(stars[y+(x*360)].delta))
    #     pass
    # pass

    # Create stars in regular intervals
    for y in range(-90,90): # declination is in [-pi/2,pi/2]
        for x in range(0,360):
            stars.append(Star(math.radians(x), math.radians(y), 
                random.randrange(1, 7)))
            # print(str(stars[y+(x*360)].alpha)+', '+str(stars[y+(x*360)].delta))
        pass
    pass

    # x_angles = [350, 355, 0, 5, 10]
    # y_angles = [-10, -5, 0, 5, 10]
    # for x in x_angles:
    #     for y in y_angles:
    #         stars.append(Star(math.radians(x), math.radians(y), 
    #             random.randrange(5, 7)))
    #         pass
    #     pass
    

    # TODO: for delta = -90 or 90 stars are counted as projected but don't show
    # up they are most likely projected on top of the others 
    for star in stars:
        coord = get_projection_coordinates(fov_alpha, fov_delta, center_alpha, 
            center_delta, star)
        # if the star is out of view
        if (coord[0] == -1 or coord[1] == -1):
            continue
        # print(coord)
        coord = (math.floor((coord[0])*xres), math.floor((coord[1])*yres))
        # print(coord)
        #cv2.circle(img, center, radius, color, thickness=1, lineType=8, shift=0)
        cv2.circle(img, coord, star.Vmag, white, -1) # -1 thick is filled
        pass
    
    # DEBUG
    global n_stars, ins_fov_ctr
    print(str(n_stars)+' stars\n'+str(ins_fov_ctr)+' inside')
    print("ratio: "+str(ins_fov_ctr/n_stars))

    cv2.namedWindow('image', cv2.WINDOW_NORMAL)
    cv2.imshow('image',img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    pass

if __name__ == "__main__":
    # main()
    for x in range(0, 361, 90):
        for y in range(-90, 91, 45):
        
            # DEBUG
            global n_stars
            n_stars = 0
            global ins_fov_ctr
            ins_fov_ctr = 0
            print("center: "+str(x)+", "+str(y))
            main(x,y)
            pass
        pass