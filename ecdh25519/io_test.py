#!/usr/bin/env python3
import serial
import sys
import random

from enum import Enum

tests = {"fsq", "fmul", "all"}

print("Program has started")

if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)

if len(sys.argv) == 3:
  if sys.argv[2] != "all":
    tests = {sys.argv[2]}


class Opcodes(Enum):
  FSQ = 2
  FMUL = 3
  RES = 4
  TME = 5
  ERR = 255

def fe_sq(a, print_time=False):
  dev.write(Opcodes.FSQ.value.to_bytes(1, 'big'))

  dev.write(a.to_bytes(32, 'big'))

  ret = int.from_bytes(dev.read(1), 'big')

  if Opcodes(ret) != Opcodes.ERR:
    p = int.from_bytes(dev.read(32), 'big')

    ret = int.from_bytes(dev.read(1), 'big')
  
    if Opcodes(ret) == Opcodes.TME:
      res = int.from_bytes(dev.read(4), 'big')
    
      if print_time:
        print("FSQ time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

  return p

def fe_mul(a, b, print_time=False):
  dev.write(Opcodes.FMUL.value.to_bytes(1, 'big'))

  dev.write(a.to_bytes(32, 'big'))
  dev.write(b.to_bytes(32, 'big'))

  ret = int.from_bytes(dev.read(1), 'big')

  if Opcodes(ret) != Opcodes.ERR:
    p = int.from_bytes(dev.read(32), 'big')

    ret = int.from_bytes(dev.read(1), 'big')
  
    if Opcodes(ret) == Opcodes.TME:
      res = int.from_bytes(dev.read(4), 'big')
    
      if print_time:
        print("FMUL time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

  return p

print("Flag - Before fsq")

x = dev.readline()
print("Flag 1 - Read something")
print(x.decode()) # 
print("Flag 2 - We've printed it")

if 'fsq' in tests:
  for i in range(10):
    a = random.randint(0, (1<<255)-19)
  
    res = fe_sq(a, True)
  
    expected = (a*a) % ((1<<255)-19)
  
    if expected != res:
      print("ERROR!")
    else:
      print("Correct!")

  print()

print("Flag - Before fmul")

if 'fmul' in tests:
  for i in range(10):
    a = random.randint(0, (1<<255)-19)
    b = random.randint(0, (1<<255)-19)
  
    res = fe_mul(a, b, True)
  
    expected = (a*b) % ((1<<255)-19)
  
    if expected != res:
      print("ERROR!")
    else:
      print("Correct!")

  print()

dev.close()