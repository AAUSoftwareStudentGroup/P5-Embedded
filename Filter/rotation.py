import pygame, sys, math, os
from pygame.locals import *
from datetime import datetime

class AccelerometerData:
    def __init__(self, time = 0, x = 0, y = 0, z = 0, rx = 0, ry = 0, rz = 0):
        self.time = time
        self.x = x
        self.y = y
        self.z = z
        self.rx = rx
        self.ry = ry
        self.rz = rz
    def lengthT(self):
        return math.sqrt(self.x ** 2 + self.y ** 2 + self.z ** 2)
    def lengthR(self):
        return math.sqrt(self.rx ** 2 + self.ry ** 2 + self.rz ** 2)
    def printa(self):
        print "X: {} Y: {} Z: {}".format(self.x, self.y, self.z)

def average(i, j):
    return (i+j)/2

pygame.init()
screen = pygame.display.set_mode((1300, 400))

"""Create the background"""
background = pygame.Surface(screen.get_size())
background = background.convert()
background.fill((0,0,0))

"""How much is one G"""
oneG = (2**15)/16

"""How much is one rotation per second"""
oneRotation = (2**15)/(2000/360)

"""File to read"""
file = open("../data/mortensKick2.csv", "r").read().split('\n')

startTicks = pygame.time.get_ticks();

"""Time in data"""
simTime = 0
line = 0

"""os.system("(vlc " + "../data/ibsenspinner.mov" + "& )")"""
while 1:
    time = pygame.time.get_ticks() - startTicks
    background.fill(0x000000)
    acc = AccelerometerData()
    while time <= simTime:    
        time = pygame.time.get_ticks() - startTicks
    while time > simTime:
        line += 1
        data = file[line].split(';')
        simTime = int(data[0])
        acc = AccelerometerData(int(data[0]), float(data[0+1])/oneG, float(data[1+1])/oneG, float(data[2+1])/oneG, float(data[3+1])/oneRotation, float(data[4+1])/oneRotation, float(data[5+1])/oneRotation);
    for event in pygame.event.get():
        if event.type == pygame.QUIT: 
            sys.exit()
        elif event.type == KEYDOWN:
            """    
            if (event.key == K_BACKSPACE):
                
            elif (event.key == K_LEFT):
                
            elif (event.key == K_RIGHT):
                
            elif (event.key == K_DOWN):"""

    """Put your filtering/whatever code here"""
    print "Accelerometer data: {} ms, \t{}\t{}\t{}".format(time, round(acc.x, 2), round(acc.y, 2), round(acc.z, 2))

    """Draw filtering/whatever code here"""
    pygame.draw.line(background, 0x00FFFF, (100, 100), (100+(acc.x*100), 100), 1)
    pygame.draw.line(background, 0x00FFFF, (300, 100), (300+(acc.y*100), 100), 1)
    pygame.draw.line(background, 0x00FFFF, (500, 100), (500+(acc.z*100), 100), 1)

    pygame.draw.line(background, 0x00FFFF, (100, 300), (100+(acc.rx*100), 300), 1)
    pygame.draw.line(background, 0x00FFFF, (300, 320), (300+(acc.ry*100), 320), 1)
    pygame.draw.line(background, 0x00FFFF, (500, 340), (500+(acc.rz*100), 340), 1)

    pygame.display.flip()
    screen.blit(background, (0, 0))   
ser.close()