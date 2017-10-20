import socket, sys, os
if os.path.exists(sys.argv[1]+".csv") and os.path.getsize(sys.argv[1]+".csv") > 0:
    print "\nWARNING: Outputfile " + sys.argv[1] + ".csv exists and is not empty. Aborting...\n"
    exit()

try:
    receivedData = True
    HOST = ''   # Symbolic name, meaning all available interfaces
    PORT = int(sys.argv[3]) # Arbitrary non-privileged port
    tick = 0
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print 'Socket created'
     
    #Bind socket to local host and port
    try:
        s.bind((HOST, PORT))
    except socket.error as msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        sys.exit()

    print "Bind success, listening on port {}".format(PORT)

    s.sendto("", (str(sys.argv[2]), 8085))
    file = open(str(sys.argv[1])+".csv", "w")
    file.write("time;x;y;z;rx;ry;rz\n")
    receivedData = False
    print "Opened outputfile: " + str(sys.argv[1]) + ".csv"
    sys.stdout.write("Receiving data\n")
    totalTime = 0
    while 1:
        rawdata = s.recvfrom(13)
        receivedData = True
        d = []
        d.append(ord(rawdata[0][0]))

        d.append(ord(rawdata[0][1])<<8|ord(rawdata[0][2]))
        d.append(ord(rawdata[0][3])<<8|ord(rawdata[0][4]))
        d.append(ord(rawdata[0][5])<<8|ord(rawdata[0][6]))

        d.append(ord(rawdata[0][7])<<8|ord(rawdata[0][8]))
        d.append(ord(rawdata[0][9])<<8|ord(rawdata[0][10]))
        d.append(ord(rawdata[0][11])<<8|ord(rawdata[0][12]))
        
        for i in range(1,len(d)):
            if(d[i] & 1<<15):
                d[i] -= 1<<16

        #print d

        for i in range(0, len(d)/7):
            j = i*7
            totalTime += d[j]
            file.write("{};{};{};{};{};{};{}\n".format(totalTime, d[j+(0+1)], d[j+(1+1)], d[j+(2+1)], d[j+(3+1)], d[j+(4+1)], d[j+(5+1)]))
        #sys.stdout.write('.')
        #sys.stdout.flush()
        tick = tick + 1
        if tick % 5 == 0:
            s.sendto("1", (str(sys.argv[2]), 8085))

finally:
    print "\nClosing"
    s.close()
    if receivedData == False:
        os.remove(sys.argv[1] + ".csv")
        print "Deleting empty outputfile since no data was captured"