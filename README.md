# JPEG-Codec
# Brief
- Reference from [JPEG Tutorial by MROS](https://github.com/MROS/jpeg_tutorial)
- C++ Implementation
- Use optimizations as described below
 -- Canonical Huffman Code with pre-compute look-up index
 -- Dimension Reduction IDCT from ![O(N^4)](https://render.githubusercontent.com/render/math?math=O(N^4)) to ![O(N^3)](https://render.githubusercontent.com/render/math?math=O(N^3))
 -- In place swap dezigzag
# File structure
- Decoder.cpp - Decode compressed data using de-quantization, de-Zigzag, Inverse DCT, Upsampling
# Output
- Use .ppm as output format
# TODO
- handle RSTn in compressed data section
