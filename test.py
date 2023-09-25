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

for i in range(10):
  opc = random.choice([0,1,2,3])

  print(opcodes[opc], end =" ")

  dev.write(opc.to_bytes(1, 'big'))

  x = dev.readline()
  print(x.decode())


