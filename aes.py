#!/usr/bin/env python3
import serial
import sys

import random

from Crypto.Cipher import AES


if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)

print("started the encryption thing")

# run ten tests
for i in range(10):
  x = dev.readline()
  print("Flag 1 - Read something")
  print(x.decode()) # 
  print("Flag 2 - We've printed it")


  # generate random 128 bit key and random 128 bit data block
  key = bytes(random.getrandbits(8) for _ in range(16))
  data = bytes(random.getrandbits(8) for _ in range(16))

  thinke = bytes(random.getrandbits(8) for _ in range(1))
  thinke2 = bytes(random.getrandbits(8) for _ in range(2))
  thinke3 = bytes(random.getrandbits(8) for _ in range(3))
  thinke4 = bytes(random.getrandbits(8) for _ in range(4))

  print("Flag 3 - writing data")
  # send key and data to device
  a = 16
  b = 5
  c = 37
  d = 137

  print(f"a = {a}")
  print(f"b = {b}")
  print(f"c = {c}")
  print(f"d = {d}\n")

  print(f"[w] a = {a}")
  dev.write(a)
  print(f"[r] a = {dev.read(4)}")
  print(f"[w] b = {b}")
  dev.write(b)
  print(f"[r] b = {dev.read(4)}")
  print(f"[w] c = {c}")
  dev.write(c)
  print(f"[r] c = {dev.read(4)}")
  print(f"[w] d = {d}")
  dev.write(d)
  print(f"[r] d = {dev.read(4)}")

  dev.write(thinke)

  x = dev.readline()
  print("Flag 4 - Read something")
  print(x.decode()) # 
  print("Flag 5 - We've printed it")



  dev.write(key)
  dev.write(data)
  print("Flag 6 - done writing data")


  x = dev.readline()
  print("Flag 7 - Read something")
  print(x.decode()) # 
  print("Flag 8 - We've printed it")


  print("Flag 9 - reading data")

  # read cipher text from device
  ciphertext_dev = dev.read(16)
  print("Flag 10 - done reading data")

  # generate reference cipher block
  cipher = AES.new(key, AES.MODE_ECB)
  ciphertext_rev = cipher.encrypt(data)


  # compare reference and response block byte-by-byte
  for i in range(16):
    if ciphertext_dev[i] != ciphertext_rev[i]:
      print("Error in encryption!\n")
      sys.exit(-1)
 
