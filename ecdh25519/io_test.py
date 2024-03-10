#!/usr/bin/env python3
import serial
import sys
import random

from enum import Enum

tests = {"smul", "bmul", "fsq", "fmul", "rand", "all"}

if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)

if len(sys.argv) == 3:
  if sys.argv[2] != "all":
    tests = {sys.argv[2]}


class Opcodes(Enum):
  SMUL = 0
  BMUL = 1
  FSQ = 2
  FMUL = 3
  RES = 4
  TME = 5
  ERR = 255


kat_bmul = [(0x85d9a258c51cfcb55fc5ac566f3979c3e290c6f15b3a44fb708dd79376a07ab1,
             0x51ed27b3d0412346da2193ff95599de7ef750dab8113d7127671d352aaf604dc),
            (0x69e8ab5453b24a379833f8e3959b608aed72ca7300cccd427d9ba11000042000,
             0x623e7606943f627d994c2be9b52c63bdaff7c3f6c232b8bcd753a853496d32be),
            (0x577a60e74078dec320207a7866687fd5dcb2af847b5ea3f9993f30ab92bfdd30,
             0x7d6a1d618f83cb5d6d0251e79f0910ccb5868487b7347b4257fe69f482b73a2a),
            (0x7c75b0445258464d68f777d94cb1151a70a6135ad9f5998a2fb2302a1bb068ef,
             0x24228fd9c84a5ec9a2222d655066ac2a98f8e83f2a46f2dfe25e95b0717f565e),
            (0x047dd9a4ec9a0bce681ca4056b06dff50a705e824994d2a7d2778e9106421c46,
             0x0f80310b0204d74aaebbdd5565ec316e894b28c1239a64052a97eb51428d93a0),
            (0x7758b90807ca710985407e0dd18956169a6babca3cf733abfb2a6fb30f538f9d,
             0x4db26640bbc3809f603a654275d99019f12dec224d72554bcec630c398d4069c),
            (0x05f972794fbcdfdde677c5c1d4569ccf6ccd06cb1fcc9817f5eaed96540baf65,
             0x024b1706b8a690d630453f61757fc0365a593b3b67d2035ce77ce75a93dd84fa),
            (0x2c3f092e702afd789db11b6c46ff509fe2d819cc95d8ce94189a5e83ab8eedac,
             0x3864d44c0eb628f334b7d32f91c2fc2166503abef39f499e9272d6244efa5cc4),
            (0x15e6edd63e539fe8faf0cb3ba8518890b1e00db0d65e032d21307522e7f05f96,
             0x03f462d631ea72908e52fa1026cde1eec76bea1fe781ae5a8e2d3e8621c73002),
            (0x2102e0af18bd96527cf2c740efe1d0af743e330f4786196ed5add8fe94ee23bc,
             0x02e094b29fe3aecb8d8ddf3952d7299825fb6eb944901b3011afa88a6c2890bc)]

kat_smul = [(0x693fa91e0c660ecb022777f50553af85c0e4a9137078d5e23124a5e900c4dca5,
             0x31dc7678c2b2224a3e89ce281905b08dd62c35abef74bcd6cee1714de517b4b0,
             0x78cb1827ee6668ea3615c3366a271634b64ddcb97908ecb335d061dca561d896),
            (0x19ac00ac453fad19dfeef39530edbe0fd079531576462df116a4d6e1e2740875,
             0x44f9b12fe8b79062c0e52f16006a0446d36b3d05474ac05ce4c4c86698cba5ce,
             0x78784b250e21df55dfb27a823a11e3927d335153ec0a853f8348ef4daf665796),
            (0x787205b8ff329731a3ae34397375656626f99cf8541f28ac01ba964c92148ca3,
             0x405bd83aa71c480c53e48c95a74a8903ef9646be839876d088042d4ff965b754,
             0x547814d10039ff6f1a03d197a38bb87c26e79f269d79a1a7b09ea32810584efc),
            (0x4ce724b835a3b4c6d0529125f4c8c7400c34606b9cf196fd40c291269fec8314,
             0x4b61a104bbe8d8c8f74834c49e73ad7997e48cd0e8165615305dd4b5b9505872,
             0x37c3fd4fc009eb1908331b917d7b6e3a3b9738df36b283fe48bd31ab171c08d6),
            (0x1d91e7ec46a5ac5a02640280066ffaf9dba0ecad46a0eee921375eeab080f761,
             0x20d3fe9a504ec0dfdcd31c349f8c4940982a73228fc96618d42ff01e3546ac66,
             0x0ec783647e408dd709d2ec77cd287fd98e8e5b7e49f2129b7a479fdef25a635a),
            (0x7148e8d5a7510080f26d239d7361485320495efc0dc3a2ebaaff7fb8554241da,
             0x52df6cbc9b365af185841a42713c3214563c1c2c0c6cf197f48b6416ece007ae,
             0x6ae3e8f3a705ec849747c84e734c807c2d9ed4ddd56ccb115c4e12d9caa42854),
            (0x58ccafa0bcc4439721270cad2b2ba7e1f98bef4c210c4c90897cf10e064971fb,
             0x36112330005e90b8854c33fafdf75fe42b3c1bfa9d6645f42a8b11c91a69dbc2,
             0x3f2729f8e2767773610cae48e12b160ac72c4d022373fed686630c1459e1353a),
            (0x4793c4f3640d54d45ebfadd43495901093641a7f66be3b1e906116e86a42b72f,
             0x548f8112e131b1adc08de876f187341317140a44e9085bf0764828e995ba80e2,
             0x0f2b828633dfc2ee1365b3694e378fda6cd3dbe2f8f4685996418d389c00b980),
            (0x4bddd6de363f1b4071b983e3edc0172f4bff8e68147a144331548bc5c408829a,
             0x7063c9f9d1f2266c2f3f74d8989710ebab97188dbd1037012b0b4c6285edf40a,
             0x14a2e2f17a3943798ff2a7c92eeccc462176cbeffd271dd428ebc4945af89d30),
            (0x21c3d180ed7be149fcb09b810c93e25be937e28602e7baefdc9a8baa422f3fbc,
             0x4b605a6d538124e83cc9828a1f5d95c3869104ed007192f95525182b9de23904,
             0x5359fa1fe48cca2c5a052c4d2bfe0ca48b1327ee77c31358485231baae100714)
]

def base_mul(s, print_time=False):
  dev.write(Opcodes.BMUL.value.to_bytes(1, 'big'))

  dev.write(s.to_bytes(32, 'big'))

  ret = int.from_bytes(dev.read(1), 'big')

  if Opcodes(ret) != Opcodes.ERR:
    p = int.from_bytes(dev.read(32), 'big')

    ret = int.from_bytes(dev.read(1), 'big')
  
    if Opcodes(ret) == Opcodes.TME:
      res = int.from_bytes(dev.read(4), 'big')
    
      if print_time:
        print("BMUL time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

  return p


def scalar_mul(s, p, print_time=False):
  dev.write(Opcodes.SMUL.value.to_bytes(1, 'big'))

  dev.write(s.to_bytes(32, 'big'))
  dev.write(p.to_bytes(32, 'big'))

  ret = int.from_bytes(dev.read(1), 'big')

  if Opcodes(ret) != Opcodes.ERR:
    p = int.from_bytes(dev.read(32), 'big')

    ret = int.from_bytes(dev.read(1), 'big')
  
    if Opcodes(ret) == Opcodes.TME:
      res = int.from_bytes(dev.read(4), 'big')
    
      if print_time:
        print("SMUL time:", res)
    else:
      print("ERROR!\n")
  else:
    print("ERROR!\n")

  return p


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


if 'bmul' in tests:
  for s,p in kat_bmul:
    res = base_mul(s, True)
  
    if (p != res):
      print("(0x{0:064x}, 0x{1:064x}),".format(p, res))
  
  print()
  

if 'smul' in tests:
  for s,p,r in kat_smul:
    res = scalar_mul(s, p, True)
  
    if (r != res):
      print("(0x{0:064x}, 0x{1:064x}),".format(r, res))
  
  print()
  

if 'rand' in tests:
  for i in range(10):
    a = random.randint(0, (1<<255)-19)
    A = base_mul(a)
  
    b = random.randint(0, (1<<255)-19)
    B = base_mul(b)
  
    sa = scalar_mul(a, B)
  
    sb = scalar_mul(b, A)
  
    if (sa == sb):
      print("Succes!")
    else:
      print("ERROR!")

dev.close()
