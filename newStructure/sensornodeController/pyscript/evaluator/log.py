#!/usr/bin/python

import socket
import signal
import sys, time

udpPort = 8085

localIP = socket.gethostbyname(socket.gethostname())
# remove last part of ip : 192.168.1.132 -> 192.168.1.{}
subnet = localIP[::-1].split('.', 1)[1][::-1] + '.{}'
deviceIP = subnet.format(1)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

#Bind socket to local host and port
try:
    sock.bind(('', udpPort))
except sock.error as msg:
    print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
    sys.exit()


while(True):
    sock.sendto(".", (deviceIP, udpPort))
    rawdata,(senderIp,_) = sock.recvfrom(64)
    print(rawdata)

    redrawWait = int(round(time.time() * 1000))+1000/30
    while redrawWait > int(round(time.time() * 1000)):
        pass