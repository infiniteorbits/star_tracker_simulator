#!/usr/bin/python3

# python sketch of the ST Sim algo
import math
import random
import numpy as np 
import cv2
import os, sys

ins_fov_ctr = 0
n_stars = 0

##################### Helper functions #########################################

# limit check for naive implementation of the projection
def between(x, lower_lim, upper_lim):
    if (x<lower_lim or x>upper_lim):
        return False    
    return True

def inside(x, center, fov):
    if (x >= math.pi):
        return between(x-2*math.pi, (center-(fov/2)), (center+(fov/2)))
    return between(x, (center-(fov/2)), (center+(fov/2)))

def random_angle_in_deg():
    return random.uniform(0, 360)

def random_angle_in_rad():
    return random.uniform(0, 2*math.pi)

################################################################################


class Star:
    """honestly not much of a reason rn but might be useful later"""
    def __init__(self, star_alpha, star_delta, Vmag):
        global n_stars
        n_stars += 1
        # super(Star, self).__init__()
        self.Vmag = Vmag
        self.alpha = star_alpha
        self.delta = star_delta


def get_projection_coordinates(fov_alpha, fov_delta, center_alpha, center_delta, 
    st_alpha, st_delta):
    # all in radians
    
    # check whether the star is inside the field of view
    if (not(inside(st_alpha, center_alpha, fov_alpha)) or
        not(inside(st_delta, center_delta, fov_delta))):
        return (-1, -1) # star is outside the field of view
        pass
    
    global ins_fov_ctr
    ins_fov_ctr += 1

    star_rel_alpha = st_alpha-center_alpha
    star_rel_delta = st_delta-center_delta

    # check whether fov is 180 deg or multiples
    # if (fov_alpha % math.pi == 0):
    #     sys.exit("fov is "+str(fov_alpha)+" rad")
    #     pass


    # TODO what's the solution with divide by zero when fov is 180 deg (see above)
    x = (math.tan(star_rel_alpha)/(2*math.tan(fov_alpha/2))) +0.5
    y = (math.tan(star_rel_delta)/(2*math.tan(fov_delta/2))) +0.5
    print(x,y)
    return (x, y)

# looking for [x,y] where x, y are floats b/w -0.5 and 0.5 indicating their position
# unto the image plane



##################### Main  ####################################################

def main():
    
    # given angles on the celestial sphere
    fov_alpha = math.radians(90) # i'll call this the x, relative span of afov, right ascension
    fov_delta = math.radians(90) # i'll call this the y, relative span of afov, declination
    center_alpha = math.radians(90) # from 0 to 360 deg
    center_delta = math.radians(90) # from 0 to 180 deg
    
    # Create a black image
    xres = 2048
    yres = 2048
    img = np.zeros((xres, yres, 3), np.uint8)
    white = (255,255,255)

    stars = []
    # # Create random stars
    # for x in range(0,100000):
    #     stars.append(Star(random_angle_in_rad(), random_angle_in_rad(), 
    #         random.randrange(7)))
    #     # print(stars[x].alpha)
    #     pass

    # Create stars in regular intervals
    for x in range(0,180): # normally declination is in [-pi/2,pi/2]
        for y in range(0,360):
            stars.append(Star(math.radians(y), math.radians(x), 
                random.randrange(7)))
            # print(str(stars[y+(x*360)].alpha)+', '+str(stars[y+(x*360)].delta))
        pass
    pass

    for star in stars:
        coord = get_projection_coordinates(fov_alpha, fov_delta, center_alpha, 
            center_delta, star.alpha, star.delta)
        # if the star is out of view
        if (coord[0] == -1 or coord[1] == -1):
            continue
        # print(coord)
        coord = (math.floor((coord[0])*xres), math.floor((coord[1])*yres))
        # print(coord)
        #cv2.circle(img, center, radius, color, thickness=1, lineType=8, shift=0)
        cv2.circle(img, coord, star.Vmag, white, -1) # -1 thick is filled
        pass
    
    global ins_fov_ctr
    print(str(n_stars)+' stars\n'+str(ins_fov_ctr)+' inside')
    print("ratio: "+str(ins_fov_ctr/n_stars))

    cv2.namedWindow('image', cv2.WINDOW_NORMAL)
    cv2.imshow('image',img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    pass

if __name__ == "__main__":
    main()