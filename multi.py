#!/usr/bin/env python3
import serial
import sys
import random

if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)

opcodes = {
  0 : "ADD",
  1 : "MUL",
  4 : "RES",
  5 : "TME",
  255 : "ERR"}

print("Starting calculations")

for i in range(10):
  x = dev.readline()
  print("Flag 1 - Read something")
  print(x.decode()) # 
  print("Flag 2 - We've printed it")

  opc = random.choice([0,1])

  print(opcodes[opc], end =" ")

  dev.write(opc.to_bytes(1, 'big'))

  x = dev.readline()
  print("Flag 3 - Read something")
  print(x.decode()) # 
  print("Flag 4 - We've printed it")


  a = random.randint(0, 1<<256-1)
  print(f"a = {a}")
  print(f"a binary = {bin(a)}")

  dev.write(a.to_bytes(32, 'big'))
  print("Flag 5 - sent data over")

  x = dev.readline()
  print("Flag 6 - Read something")
  print(x.decode()) # 
  print("Flag 7 - We've printed it")

  b = random.randint(0, 1<<256-1)

  dev.write(b.to_bytes(32, 'big'))


  ret = int.from_bytes(dev.read(1), 'big')


  if opcodes[ret] != "ERR":
    res = int.from_bytes(dev.read(64), 'big')
   
    if opcodes[opc] == "ADD":
      print((a+b) == res, end =" ")


    if opcodes[opc] == "MUL":
      print((a*b) == res, end =" ")


    ret = int.from_bytes(dev.read(1), 'big')
  
    if opcodes[ret] == "TME":
      res = int.from_bytes(dev.read(4), 'big')
    
      print("  time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

dev.close()
  