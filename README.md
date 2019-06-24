# PiNG12RAW

PiNG12RAW is a simple utility to separate RG/GB Bayer color channels to individual files. It takes in input a RAW12 file and outputs png files corresponding to different Bayer Channels.

### To build/make the C++ program

PiNG12RAW can be made to work easily using the following set of commands:
```
make
bin/DebayerImage.out
```
You will be asked to select Debayering algorithm 
1. LINEAR
2. BILINEAR

After running these commands and selecting desired algorithm, one can see the debayered colored channels in `results` folder.
