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



while True:
  print("---| Begin loop |---")
  print("More text to check")
  x = dev.readline()
  print("Flag 1 - Read something")
  print(x.decode()) # 
  print("Flag 2 - We've printed it")

  opc = random.choice([0,1,2,3])
  print(opcodes[opc], end =" ")
  print("printed opcodes")
  dev.write(opc.to_bytes(1, 'big'))

  y = dev.readline()
  print("Flag 3 - Read something")
  print(y.decode()) # 
  print("Flag 4 - We've printed it")


  operandA = 7
  operandB = 3
  dev.write(operandA.to_bytes(4, 'big'))
  dev.write(operandB.to_bytes(4, 'big'))
  print("Flag 5 - Sent operands")

  y = dev.readline()
  print("Flag 6 - Read something")
  print(y.decode()) # 
  print("Flag 7 - We've printed it")







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

