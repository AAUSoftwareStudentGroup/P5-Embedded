import socket, sys
try:

    HOST = ''   # Symbolic name, meaning all available interfaces
    PORT = 8085 # Arbitrary non-privileged port
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


    s.sendto("GO", ('192.168.43.252', 8085))

    file = open(str(sys.argv[1])+".csv", "w")
    file.write("time;x;y;z;rx;ry;rz\n")
    print "Opened outputfile: " + str(sys.argv[1]) + ".csv"
    sys.stdout.write("Receiving data\n")
    totalTime = 0
    while 1:
        rawdata = s.recvfrom(13)
        while rawdata == "":
            #conn, addr = s.accept()
            print "reconnected to {}".format(addr)
            #rawdata = conn.recv(100*7*16)
        data2 = []
        data2.append(ord(rawdata[0][0]))

        data2.append(ord(rawdata[0][1])<<8|ord(rawdata[0][2]))
        data2.append(ord(rawdata[0][3])<<8|ord(rawdata[0][4]))
        data2.append(ord(rawdata[0][5])<<8|ord(rawdata[0][6]))

        data2.append(ord(rawdata[0][7])<<8|ord(rawdata[0][8]))
        data2.append(ord(rawdata[0][9])<<8|ord(rawdata[0][10]))
        data2.append(ord(rawdata[0][11])<<8|ord(rawdata[0][12]))
        
        for i in range(1,len(data2)):
            if(data2[i] & 1<<15):
                data2[i] -= 1<<16

        print data2

        for i in range(0, len(data2)/7):
            j = i*7
            totalTime += data2[j]
            file.write("{};{};{};{};{};{};{}\n".format(totalTime, data2[j+(0+1)], data2[j+(1+1)], data2[j+(2+1)], data2[j+(3+1)], data2[j+(4+1)], data2[j+(5+1)]))
        #sys.stdout.write('.')
        sys.stdout.flush()

finally:
    print "\nClosing"
    s.close()