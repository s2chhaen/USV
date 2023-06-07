import csv
import numpy as nmp
from FunctionsToUse.CartFunctions import * # importing direct, corr, cart2pol, cart_angl, cart_dist

# read + push to array
def mission_read(rows, fileName):
    with open(fileName, newline='') as mission:
        
        y = 2
        
        mission_read = csv.reader(mission, delimiter='\t')

        wp_cart = nmp.zeros((rows - 1, y))
        
        for mission in mission_read:

            if mission_read.line_num > 1:

                wp_cart[mission_read.line_num - 2, 0] = float(mission[8])
                wp_cart[mission_read.line_num - 2, 1] = float(mission[9])
                
                speed = float(mission[2])       #moeglicherweise falsche zeile
                epsilon = float(mission[5])

        #print('\n wp_cart')
        #print(wp_cart)

        return wp_cart, speed, epsilon

class mission:

    offset = 3
    
    pol = nmp.zeros([2])
    corr_pol = nmp.zeros([2])
    corr_cart = nmp.zeros([2])
    
    origin_corr_cart = nmp.zeros([2])
    
    punktB = [0.0,0.0]          # wo wir hin wollen
    punktA = [0.0,0.0]          # wo wir herkommen

    prevIndex = 0

    def __init__(self, filename):
        
        # lenght mission.txt for array
        with open(filename, newline='') as init:
            
            init_read = csv.reader(init)
            
            #counting
            for row in init_read:
                pass
            
            self.rows = init_read.line_num
            
        self.wp_cart, self.speed, self.epsilon = mission_read(self.rows,filename)
        
        self.punktA = self.wp_cart[0]
        self.punktB = self.punktA           # anfang -> punkt A auf punktB setzen

        self.origin_corr_cart = self.wp_cart[0]


    def update(self, gps, comp:float, index:int, lock):

        # self.speed = 0              #non permanent, nach testen entfernen




        self.pol = cart2pol(gps, self.wp_cart[index], comp)
        
        # decision for direction within scanned area
        if abs(2*self.pol[1]) < 180:
            
            self.sector_wp = int(180 + 2*self.pol[1])
            
            #print('sector_wp', self.sector_wp)
            
            if lock[self.sector_wp] == False:
                
                self.corr_pol[0] = self.pol[0]
                self.corr_pol[1] = corr(self.sector_wp, lock)
            
            else:
                # not locked
                self.corr_pol = self.pol

                #print('no change')

        else:
            # actual direction not within scanned area
            self.corr_pol = self.pol

            #print('no change')


        # print('corr_pol', self.corr_pol)
        
        
        
        ###RETURN
        self.corr_cart = direct(gps, self.corr_pol, comp)

        if index == self.rows-1:                # end bedingung
            self.punktA = self.punktB
        elif self.prevIndex != index:           # naechster wegpunkt
            self.punktA = self.punktB
            self.punktB = self.corr_cart

        

        #print('corr_cart: ', self.corr_cart)

        self.prevIndex = index