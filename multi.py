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
  print("\n-----")
  x = dev.readline()
  #print("Flag 1 - Read something")
  print(x.decode()) # 
  #print("Flag 2 - We've printed it")

  opc = random.choice([0,1])
  opc = 1; #-----------------------------------------------------------------

  print(opcodes[opc], end =" ")

  dev.write(opc.to_bytes(1, 'big'))

  x = dev.readline()
  #print("Flag 3 - Read something")
  print(x.decode()) # 
  #print("Flag 4 - We've printed it")


  a = random.randint(0, 1<<256-1)
  b = random.randint(0, 1<<256-1)
  #print(f"a = {a}")
  #print(f"b = {b}")
  #print(f"a binary = {bin(a)}")
  #print(f"b binary = {bin(b)}")

  dev.write(a.to_bytes(32, 'big'))
  dev.write(b.to_bytes(32, 'big'))
  #print("Flag 5 - sent data over")

  x = dev.readline()
  #print("Flag 6 - Read something")
  print(x.decode()) # 
  #print("Flag 7 - We've printed it")

  ret = int.from_bytes(dev.read(1), 'big')

  #print("Flag 8 - Gotten opcode back")

  print(f"round = {i}")
  if opcodes[ret] != "ERR":
    res = int.from_bytes(dev.read(64), 'big')
   
    if opcodes[opc] == "ADD":
      print("Flag  9 - ADD")
      print(f"res = {res}")
      print(f"oth = {a+b}")
      print((a+b) == res, end =" ")


    if opcodes[opc] == "MUL":
      print("Flag  9 - MUL")
      print(f"limb = {int(i)}")
      print(f"  a = {bin(a)}")
      print(f"  b = {bin(b)}")
      print(f"res = {bin(res)}")
      print(f"oth = {bin(a*b)}")
      print((a*b) == res, end =" ")

    #print("Flag  10 - Done with calcs")
    ret = int.from_bytes(dev.read(1), 'big')
    #print("Flag  11 - Got muh code")
  
    if opcodes[ret] == "TME":
      res = int.from_bytes(dev.read(4), 'big')
      #print("Flag  12 - Got muh time as well")
    
      print("  time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

dev.close()
  