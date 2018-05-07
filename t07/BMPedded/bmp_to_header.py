#!/usr/bin/python3
from PIL import Image
import os, sys, io
#import huffman

mask_5_bits = 0x1F
mask_6_bits = 0x3F

image_data_c = """uint8_t image_data[] = { """

def encode_image(data):
  global image_data_c
  image_data_c += ', '.join([hex(byte) for byte in data])
  image_data_c += ' };'
  return image_data_c

if __name__=="__main__":
  for infile in sys.argv[1:]:
    raw_input_file = io.FileIO(infile, mode='r')
    image_string = encode_image(raw_input_file.readall())
    with open('image_data.h', 'w') as output_file:
      output_file.write(image_string)

