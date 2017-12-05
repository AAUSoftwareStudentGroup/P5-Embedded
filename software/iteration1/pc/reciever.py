#!/usr/bin/python

import socket
import signal
import sys, time

PORT = 8085
IP = 165

localIP = socket.gethostbyname(socket.gethostname())
# remove last part of ip : 192.168.1.132 -> 192.168.1.{}
subnet = localIP[::-1].split('.', 1)[1][::-1] + '.{}'
apip = subnet.format(IP)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

#Bind socket to local host and port
try:
    sock.bind(('', PORT))
except sock.error as msg:
    # print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()

running = True
while(running):
    # Key options
    sock.sendto('a', (apip, PORT))
    rawdata,(senderIp,_) = sock.recvfrom(1024)
    data = rawdata.split(';');
    t = float(data[0])/1000;
    x = float(data[1]);
    y = float(data[2]);
    z = float(data[3]);
    rx = float(data[4]);
    ry = float(data[5]);
    rz = float(data[6]);
    print("{:10.4f}, {:10.4f}, {:10.4f}, {:10.4f}, {:10.4f}, {:10.4f}, {:10.4f}".format(t,x,y,z,rx,ry,rz))
    # print(rawdata)

    # redrawWait = int(round(time.time() * 1000))+1000/30
    # while redrawWait > int(round(time.time() * 1000)):
        # pass
