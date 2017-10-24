import pygame, sys, math
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

sampleRate = 6
first = True
for arg in sys.argv:
    print "Running"
    if arg != sys.argv[0]:
        """File to read"""
        inputFile = open(str(arg), "r").read().split('\n')
        outputFile = open(str(arg) + ".scale{}.csv".format(sampleRate), "w")
        
        print "Opening {} for scaling".format(str(arg))

        startTicks = pygame.time.get_ticks();

        lineCount = 0
        accaverage = AccelerometerData()
        outputFile.write("time;x;y;z;rx,ry;rz\n")
        acc = AccelerometerData()
        
        for line in inputFile:
            data = line.split(';')
            lineCount += 1
            if data[0] != "time" and data[0] != "":
                acc = AccelerometerData(int(data[0]), int(data[0+1]), int(data[1+1]), int(data[2+1]), int(data[3+1]), int(data[4+1]), int(data[5+1]));
                accaverage += acc
                if lineCount % sampleRate == 0:
                    accaverage.scale(sampleRate)
                    outputFile.write("{};{};{};{};{};{};{}\n".format(acc.time, acc.x, acc.y, acc.z, acc.rx, acc.ry, acc.rz))
    else:
        print "Not taking this one"
    print "Done"