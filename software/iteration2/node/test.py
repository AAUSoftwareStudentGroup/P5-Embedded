import serial

class datapoint:
	X = 0.0
	Y = 0.0
	Z = 0.0
	RX = 0.0
	RY = 0.0
	RZ = 0.0

def ReadDatapoint(fo):
	return fo.readline().strip()
def ReadResult(fo):
	return fo.readline().strip()

fo = open("anton-3-morten-10-1.csv")
fo_res = open("anton-3-morten-10-1.result.csv")

ser = serial.Serial('/dev/ttyUSB0', 115200)  # open serial port

print(ser.name)         # check which port was really used

if fo.readline() != "time;x;y;z;rx;ry;rz\n":
	print("File did not start with time:x...")

newByte = ""
line = ""
while True:
	newByte = ser.read(1)
	if newByte == '#' and line.strip().endswith("START"):
		break
	elif newByte == "\n":
		print "Not 'START#': '" + line.strip() + "'"
		line = ""
	line += newByte
# Connection made and ESP program started
print "Starting..."

cont = True
while cont:
	line = ""
	newByte = ""
	while newByte != '\n':
		newByte = ser.readline(1)
		line += newByte;
	line = line.strip()
	if line.startswith("COMMAND#"):
		command = line.split('#')[1]
		if command == "SEND_DATAPOINT":
			#print "PC: Sending datapoint"
			ser.write(b"DATAPOINT#")
			outp = map(ord,ReadDatapoint(fo)+";\n")
			if outp == '':
				cont = False;
			ser.write(outp)
			#print outp
		elif command == "SEND_RESULT":
			#print "PC: Sending result"
			ser.write(b"RESULT#")
			ser.write(bytearray(ReadResult(fo)+'\n',"UTF-8"))	
	elif line.startswith("DEBUG#"):
		print "ESP: " + line.split('#')[1]
	elif line.startswith("START#"):
		print "PC: Start received after program start"
	else:
		print "PC: Failed to recoqnise transmission: '" + line + "'"

ser.close() 