import pygame, sys, math, graph
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
    def __iadd__(self, other):
        self = self + other
        return self
    def __add__(self, other):
        self.time = self.time + other.time
        self.x = self.x + other.x
        self.y = self.y + other.y
        self.z = self.z + other.z
        self.rx = self.rx + other.rx
        self.ry = self.ry + other.ry
        self.rz = self.rz + other.rz
        return self
    def scale(self, other):
        if isinstance(other, int):
            self.time = self.time / other
            self.x = self.x / other
            self.y = self.y / other
            self.z = self.z / other
            self.rx = self.rx / other
            self.ry = self.ry / other
            self.rz = self.rz / other
            return self

    def __itruediv__(self, other):
        self = self / other
        return self

def average(i, j):
    return (i+j)/2

pygame.init()

screen = pygame.display.set_mode((1600, 900))

"""Create the background"""
background = pygame.Surface(screen.get_size())
background = background.convert()
background.fill((0,0,0))

"""How much is one rotation per second"""
oneRotation = (2**15)/(2000/360)

"""File to read"""
file = open("../data/mortenKick.csv", "r").read().split('\n')

# time since 1970, used to make animation run in real-time instead of all at once (difference between startTicks before running and while running)
startTicks = pygame.time.get_ticks();

"""Time in data"""
simTime = 0
line = 0
# last node is the previous node's end position, which will be used as start position for the next
lastNode = (0, 0)

sampleRate = 6
accaverage = AccelerometerData()

acc = []

for line in file:
    data = line.split(';')
    if data[0] != "time" and data[0] != "":
        acc.append(AccelerometerData(float(data[0]), float(data[0+1]), float(data[1+1]), float(data[2+1]), float(data[3+1]), float(data[4+1]), float(data[5+1])))
graph = graph.Graph(acc, 1200, 600, 20, 1/float(16))
ctrlDown = False
graphPosx = 100 
while 1:
    time = pygame.time.get_ticks() - startTicks
    background.fill(0x000000)
    #acc = AccelerometerData()

    #while time > simTime:
    for event in pygame.event.get():
        if event.type == pygame.QUIT: 
            sys.exit()
        elif event.type == MOUSEBUTTONDOWN:
            if event.button == 5: # Scrolling down 
                if ctrlDown:
                    graph.SetScale(0.9, event.pos[0]-graphPosx)
                else:
                    graph.SetDataHeight(0.9)
            elif event.button == 4: # Scrolling !down
                if ctrlDown:
                    graph.SetScale(1.1, event.pos[0]-graphPosx)
                else:
                    graph.SetDataHeight(1.1)
        elif event.type == KEYDOWN: 
            if event.key == K_LEFT:
                graph.SetDataOffset(-10)
            elif event.key == K_RIGHT:
                graph.SetDataOffset(10)
            elif event.key == K_LCTRL:
                ctrlDown = True
        elif event.type == KEYUP: 
            if event.key == K_LCTRL:
                ctrlDown = False
        elif event.type == pygame.MOUSEMOTION:
            if event.buttons[0]:
                print graph.scale
                graph.SetDataOffset(-event.rel[0]/graph.scale)
    
    graph.Render(background, graphPosx, 100)

    screen.blit(background, (0, 0))   
    pygame.display.flip()