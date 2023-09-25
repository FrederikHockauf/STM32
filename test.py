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
  2 : "EXP",
  3 : "INC",
  4 : "RES",
  5 : "TME",
  255 : "ERR"}

single_operand = ["INC"]

print("before the loop")

for i in range(10):
  print("in the loop")
  opc = random.choice([0,1,2,3])

  print(opcodes[opc], end =" ")

  dev.write(opc.to_bytes(1, 'big'))

  a = random.randint(0, 1<<32-1)

  dev.write(a.to_bytes(4, 'big'))

  b = -1

  if opcodes[opc] not in single_operand:
    b = random.randint(0, 1<<32-1)

    dev.write(b.to_bytes(4, 'big'))

  ret = int.from_bytes(dev.read(1), 'big')

  if opcodes[ret] != "ERR":
    res = int.from_bytes(dev.read(4), 'big')

    if opcodes[opc] == "ADD":
      print("back and ADD'ing --- to be implemented")
      #print((a+b) & 0xffffffff == res, end =" ")

    if opcodes[opc] == "MUL":
      print("back and MULT'ing --- to be implemented")
      #print((a*b) & 0xffffffff == res, end =" ")

    if opcodes[opc] == "EXP":
      print("back and EXP'ing --- to be implemented")
      #print(pow(a, b, 1<<32) == res, end =" ")

    if opcodes[opc] == "INC":
      print("back and INC'ing --- to be implemented")
      #print((a+1) & 0xffffffff == res, end =" ")

    ret = int.from_bytes(dev.read(1), 'big')
    print("Dis is da opcode agen = ", ret)
  
    if opcodes[ret] == "TME":
      res = int.from_bytes(dev.read(4), 'big')
      print("  time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

  x = dev.readline() #---------------------- Don't forget these
  print(x.decode()) #---------------------- Don't forget these


dev.close()



