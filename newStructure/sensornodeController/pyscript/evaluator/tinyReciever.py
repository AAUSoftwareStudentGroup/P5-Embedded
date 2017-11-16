#!/usr/bin/python

import curses
from Mote import *

# import subprocess, os, errno
import socket
# from threading import Thread
# from time import sleep, gmtime, strftime

import signal
import sys, time
 
def signal_term_handler(signal, frame):
    print 'got SIGTERM'
    Mote.GracefullStop()
    sys.exit(0)
 
signal.signal(signal.SIGTERM, signal_term_handler)

def main(screen):
    menuIndex = 0
    menuScroll = 0
    running = True
    
    screen.nodelay(1)
    curses.curs_set(0)

    menu = curses.newpad(300, 300)

    localIP = socket.gethostbyname(socket.gethostname())
    # remove last part of ip : 192.168.1.132 -> 192.168.1.{}
    subnet = localIP[::-1].split('.', 1)[1][::-1] + '.{}'
    apip = subnet.format(1)
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    #Bind socket to local host and port
    try:
        sock.bind(('', 8085))
    except sock.error as msg:
        # print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        sys.exit()

    while(running):
        # Key options
        sock.sendto('.', (apip, 8085))
        rawdata,(senderIp,_) = sock.recvfrom(13)

        menu.addstr(1,1,rawdata)

        screen.refresh()
        menu.refresh(menuScroll,0,0,0,curses.LINES-3,curses.COLS-1)
        
        redrawWait = int(round(time.time() * 1000))+1000/30
        while redrawWait > int(round(time.time() * 1000)):
            c = screen.getch()
            if c == ord('q'):
                running = False   
            elif c == ord('s'):
                menu.clear()
                screen.clear()


curses.wrapper(main)
