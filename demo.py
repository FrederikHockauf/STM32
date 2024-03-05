#!/usr/bin/env python3
import serial   # pySerial
import sys

if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)

while True:
  print("---| Begin loop |---")
  x = dev.readline()
  print("Flag 1 - Read something")
  print(x.decode()) # 
  print("Flag 2 - We've printed it")

  opcode = 5
  opcode_data = opcode.to_bytes(1,'big')
  dev.write(data)
  y = dev.readline()
  print("Flag 3 - Read something")
  print(y.decode()) # 
  print("Flag 4 - We've printed it")

  v = 16975083
  print("Flag 3")
 

  data = v.to_bytes(4, 'big')
  print("Flag 4")
  dev.write(data)
  print("Flag 5")

  res = int.from_bytes(dev.read(4), 'big')
  print("Flag 6")

  print((v*3 - res1) == 0)
  print("Flag 7")
  print(res)
  
  x = dev.readline()
  print("Flag 8")
  print(x.decode())
  print("---| End loop |---")

