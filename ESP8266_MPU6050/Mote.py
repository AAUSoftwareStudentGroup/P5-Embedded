import subprocess, os, errno
import socket
import curses
from threading import Thread
from time import sleep, gmtime, strftime

FNULL = open(os.devnull, 'w')

def CreateDir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def informUser(screen, message):
    panelHeight = 5
    panelWidth = 50
    panel = curses.newwin(panelHeight, panelWidth, int((curses.LINES-panelHeight)/2), int((curses.COLS-panelWidth)/2))

    panel.box()
    # panel.addstr(0, int(panelHeight/2), prompt)
    # panel.addstr(panelHeight-1, panelWidth-3-len(bottomPrompt), bottomPrompt)
    panel.addstr(int(panelHeight/2), 2, message)
    panel.refresh()
    
class Mote:
    motes = []
    samplingMotes = None
    monitorThread = None
    keepAliveThread = None
    socket = None
    killSignal = False

    """The sensor node class"""
    def __init__(self, ip):
        self.ip = ip
        self.file = None
        self.fileHandle = None
        self.tx = 0
        self.ty = 0
        self.tz = 0
        self.rx = 0
        self.ry = 0
        self.rz = 0
        self.diffTime = -1
        self.totalTime = -1

    def parseMessage(self, data):
        self.diffTime = (ord(data[0]))

        d = []
        d.append(ord(data[1])<<8|ord(data[2]))
        d.append(ord(data[3])<<8|ord(data[4]))
        d.append(ord(data[5])<<8|ord(data[6]))

        d.append(ord(data[7])<<8|ord(data[8]))
        d.append(ord(data[9])<<8|ord(data[10]))
        d.append(ord(data[11])<<8|ord(data[12]))

        # Twos complement
        for i in range(0,len(d)):
            if(d[i] & 1<<15):
                d[i] -= 1<<16
        
        self.tx = d[0]
        self.ty = d[1]
        self.tz = d[2]

        self.rx = d[3]
        self.ry = d[4]
        self.rz = d[5]

    def write(self):
        self.totalTime += self.diffTime
        self.fileHandle.write("{};{};{};{};{};{};{}\n".format(
            self.totalTime, 
            self.tx, 
            self.ty, 
            self.tz,
            self.rx,
            self.ry,
            self.rz))


    @staticmethod
    def Monitor():
        while not Mote.killSignal:
            rawdata,(senderIp,_) = Mote.socket.recvfrom(13)
            if(Mote.samplingMotes != None):
                for m in Mote.samplingMotes:
                    if m.ip == senderIp:
                        m.parseMessage(rawdata)
                        m.write()
                        break
            else:
                for m in Mote.motes:
                    if m.ip == senderIp:
                        m.parseMessage(rawdata)
                        break

    @staticmethod
    def KeepAlive():
        while not Mote.killSignal:
            if(Mote.samplingMotes != None):
                for m in Mote.samplingMotes:
                    Mote.socket.sendto('.', (m.ip, 8085))
            else:
                for m in Mote.motes:
                    Mote.socket.sendto('.', (m.ip, 8085))
            sleep(0.02)
            

    @staticmethod
    def StartMonitoring():
        Mote.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        Mote.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        #Bind socket to local host and port
        try:
            Mote.socket.bind(('', 8085))
        except socket.error as msg:
            # print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
            sys.exit()

        Mote.monitorThread = Thread(target = Mote.Monitor)
        Mote.keepAliveThread = Thread(target = Mote.KeepAlive)
        Mote.keepAliveThread.start()
        Mote.monitorThread.start()
        return

    @staticmethod
    def StartSampling():
        samplesFolder = strftime("%Y_%m_%d__%H_%M_%S", gmtime())
        samplesDir = os.getcwd()+"/Samples/"
        CreateDir(samplesDir)
        CreateDir(samplesDir+samplesFolder)
        
        try:
            os.symlink(samplesDir+samplesFolder, samplesDir+"latest")
        except OSError, e:
            if e.errno == errno.EEXIST:
                os.remove(samplesDir+"latest")
                os.symlink(samplesDir+samplesFolder, samplesDir+"latest")

        Mote.samplingMotes = []
        for m in Mote.motes:
            if m.file != None:
                m.fileHandle = open(samplesDir+samplesFolder+'/'+m.file+".csv", "w")
                m.fileHandle.write("time;x;y;z;rx;ry;rz\n")
                m.totalTime = 0
                Mote.samplingMotes.append(m)

        
            


    @staticmethod
    def GracefullStop():
        Mote.killSignal = True
        Mote.monitorThread.join()
        Mote.keepAliveThread.join()
        Mote.socket.close()


    @staticmethod
    def ReIndex(window):
        localIP = socket.gethostbyname(socket.gethostname())
        # remove last part of ip : 192.168.1.132 -> 192.168.1.{}
        subnet = localIP[::-1].split('.', 1)[1][::-1] + '.{}'
        # subnet = "127.0.0.{}"
        processes = []
        newMotes = []
        for i in range(2,255):
            testIp = subnet.format(i)
            if testIp == localIP:
                continue
            processes.append((testIp, subprocess.Popen(['ping',testIp,'-c','1',"-W","1"], stdout=FNULL, stderr=subprocess.STDOUT)))

        i = 1
        for ip,p in processes:
            informUser(window, 'pinging {} of {}'.format(i,len(processes)))
            i += 1
            p.wait()
            if p.poll() == 0:
                newMotes.append(Mote(ip))
                # motes.append(Mote(ip))
        

        # remove motes not in new set
        i = 0
        while i < len(Mote.motes):
            old = Mote.motes[i]
            exsists = False
            for new in newMotes:
                if old.ip == new.ip:
                    exsists = True
                    break

            if not exsists:
                Mote.motes.remove(old)
            else:
                i += 1
        
        # add motes not in old set
        for new in newMotes:
            exsists = False
            for old in Mote.motes:
                if old.ip == new.ip:
                    exsists = True
                    break

            if not exsists:
                Mote.motes.append(new)