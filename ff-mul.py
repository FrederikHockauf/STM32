#!/usr/bin/python

import serial
import sys
import random

from pyfinite import ffield


if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)


opcodes = {
  0 : "ADD",
  1 : "MUL",
  2 : "MUL_BS",  # multiplication with bitslicing
  4 : "RES",
  5 : "TME",
  255 : "ERR"}


# set new reduction polynomial here (gen=...) for Exercise 2b) and Exercise 3
K = ffield.FField(11, gen=0b111111111011)  # binary for x^11 + x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x + 1

print("modulus: {0} ({1}: {1:b}b)".format(K.ShowPolynomial(K.generator), K.generator)) 


for opc in [0,1]:  # add MUL_BS to the list for Exercise 3
  print("{0:6}".format(opcodes[opc]), end ="  ")
  
  # send opcode to the device
  dev.write(int(opc).to_bytes(1, 'big'))
  
  data = []
  
  # send a batch of 32 input pairs for this operation to the device
  for i in range(32):
    v0 = K.GetRandomElement()
    v1 = K.GetRandomElement()
  
    data.append((v0, v1))
  
    v0 = v0.to_bytes(4, 'big')
    dev.write(v0)
  
    v1 = v1.to_bytes(4, 'big')
    dev.write(v1)
  
  
  # receive response
  ret = int.from_bytes(dev.read(1), 'big')
  
  if opcodes[ret] == "ERR":
    print("ERROR")
  else:
    res = True

    # receive and check all 32 results
    for v0, v1 in data:
      v3 = int.from_bytes(dev.read(4), 'big')

      if opcodes[opc] == "ADD":
        res = res and (K.Add(v0,v1) == v3)
      else:
        res = res and (K.DoMultiply(v0,v1) == v3)

    print(res, end ="  ")
  

    # receive time
    ret = int.from_bytes(dev.read(1), 'big')
    
    if opcodes[ret] == "TME":
      res = int.from_bytes(dev.read(4), 'big')
    
      print("time: {0:4}  (avg.: {1:6.2f})".format(res, res/32))
    else:
      print("ERROR!\n")

