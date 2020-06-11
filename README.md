# JPEG-Codec
## Brief
* Reference from [JPEG Tutorial by MROS](https://github.com/MROS/jpeg_tutorial)
* C++ Implementation
* Use optimizations as described below

    * Canonical Huffman Code with pre-compute look-up index

    * Dimension Reduction IDCT from ![O(N^4)](https://render.githubusercontent.com/render/math?math=O(N^4)) to ![O(N^3)](https://render.githubusercontent.com/render/math?math=O(N^3))

    * In place swap dezigzag
## File structure
* Segment.cpp - Define how each segment read jpg data
* Decoder.cpp - Decode compressed data using de-quantization, de-Zigzag, Inverse DCT, Upsampling
## Output
* Use .bmp as output format
## Setup
```
mkdir -p cmake/build
cd cmake/build
cmake ../../
make
```
The generated binary will be in cmake/build directory 
## Usage
* Decode
```
main -i [input file name] (-o output file name)
```
or
```
main [input file name]
```
## Environment
* Testing
    * CPU: Intel Core i7-8750H CPU
    * Operating System: Windows 10 64-bit
    * Memory: 32GB
* Build
    * GCC Version: 6.3.0
    * CMake Version: 3.16.5
## TODO
* handle RSTn in compressed data section