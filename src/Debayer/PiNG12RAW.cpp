/* Copyright (C) 2018 Supragya Raj
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * PiNG12RAW - (https://github.com/supragya/PiNG12RAW)
 *
 */

#include "PiNG12RAW.h"

Debayer::DebayerContainer::DebayerContainer(const char *inputfilename, unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    red_Gr.resize(width * height / 4);
    grn1Gr.resize(width * height / 4);
    grn2Gr.resize(width * height / 4);
    blueGr.resize(width * height / 4);

    inputfile.open(inputfilename, std::ios::binary);
    if (!inputfile) {
        std::cerr << "Error: Cannot open input file, exiting now!" << std::endl;
        exit(1);
    }
    iBuf = new char[width * height * 3 / 2];
    inputfile.read(iBuf, width * height * 3 / 2);
    inputfile.close();

    tempstr = new char[MAXTEMPSTRLEN];
}

int Debayer::DebayerContainer::ComputeChannels() {
    int row, col;
    int iOff;
    int runCount = width * height * 3 / 2;

    for (unsigned int rPtr = 0; rPtr < runCount; rPtr += 3) {
        row = (rPtr * 2 / 3) / width;
        col = (rPtr * 2 / 3) - width * row;

        if (row % 2 == 0) {
            row = row / 2;
            col = col / 2;
            iOff = row * (width / 2) + col;
            red_Gr[iOff] = iBuf[rPtr];
            grn1Gr[iOff] = (((uint8_t) iBuf[rPtr + 1] & 0x0F) << 4 |
                            (uint8_t) iBuf[rPtr + 2] >> 4);
        } else {
            row = row / 2;
            col = col / 2;
            iOff = row * (width / 2) + col;
            grn2Gr[iOff] = iBuf[rPtr];
            blueGr[iOff] = (((uint8_t) iBuf[rPtr + 1] & 0x0F) << 4 |
                            (uint8_t) iBuf[rPtr + 2] >> 4);
        }
    }
}

int Debayer::DebayerContainer::WriteChannels(const char *fileprefix) {
    int w = width / 2;
    int h = height / 2;

    return lodepng::encode(augmented(fileprefix, RED), red_Gr, w, h, LCT_GREY) ||
           lodepng::encode(augmented(fileprefix, GR1), grn1Gr, w, h, LCT_GREY) ||
           lodepng::encode(augmented(fileprefix, GR2), grn2Gr, w, h, LCT_GREY) ||
           lodepng::encode(augmented(fileprefix, BLU), blueGr, w, h, LCT_GREY);
}

int Debayer::DebayerContainer::WriteColored(const char *filename, DebayeringAlgorithm alg) {
    std::vector<unsigned char> result;
    result.resize(3 * (width) * (height));
    int imOff, grOff;

    switch (alg) {
        case LINEAR:
            for (unsigned int i = 0; i < height; i++) {
                for (unsigned int j = 0; j < width; j++) {
                    imOff = 3 * (i * width + j);
                    grOff = (i / 2) * width / 2 + (j / 2);
                    result[imOff + 0] = red_Gr[grOff];
                    result[imOff + 1] = (grn1Gr[grOff] + grn2Gr[grOff]) / 2;
                    result[imOff + 2] = blueGr[grOff];
                }
            }
            break;
        case BILINEAR:
            int sumR,sumG,sumB;
            for (unsigned int i = 0; i < height; i++) {
                for (unsigned int j = 0; j < width; j++) {
                    imOff = 3 * (i * width + j);
                    grOff = (i / 2) * width / 2 + (j / 2);

                    sumR = 0;
                    sumG = 0;
                    sumB = 0;

                    if(i % 2 == 0 && j % 2 == 0) {

                        result[imOff + 0] = red_Gr[(i / 2) * width / 2 + (j / 2)];
                        if( j >= 1 ) {
                            sumG += grn1Gr[(i / 2) * width / 2 + ((j - 1) / 2)];
                        }
                        if( j + 1 < width) {
                            sumG += grn1Gr[(i / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        if( i >= 1) {
                            sumG += grn2Gr[((i - 1) / 2) * width / 2 + (j / 2)];
                        }
                        if( i + 1 < height) {
                            sumG += grn2Gr[((i + 1) / 2) * width / 2 + (j / 2)];
                        }
                        if( i >=1 && j >= 1) {
                            sumB += blueGr[((i - 1) / 2) * width / 2 + ((j - 1) / 2)];
                        }
                        if( i >= 1 && j + 1 < width) {
                            sumB += blueGr[((i - 1) / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        if( i + 1 < height && j >= 1) {
                            sumB += blueGr[((i + 1) / 2) * width / 2 + ((j - 1) / 2)];
                        }
                        if( i + 1 < height && j + 1 < width ) {
                            sumB += blueGr[((i + 1) / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        result[imOff + 1] = sumG / 4;
                        result[imOff + 2] = sumB / 4;

                    } else if (i % 2 == 0 && j % 2 != 0) {

                        if( j >= 1 ) {
                            sumR += red_Gr[(i / 2) * width / 2 + ( (j - 1) / 2)];
                        }
                        if( j + 1 < width ) {
                            sumR += red_Gr[(i / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        if( i >= 1) {
                            sumB += blueGr[((i - 1) / 2) * width / 2 + (j / 2)];
                        }
                        if( i + 1 < height ) {
                            sumB += blueGr[((i + 1) / 2) * width / 2 + (j / 2)];
                        }
                        result[imOff + 0] = sumR / 2;
                        result[imOff + 1] = grn1Gr[(i / 2) * width / 2 + (j / 2)];
                        result[imOff + 2] = sumB / 2;
                    } else if (i % 2 != 0 && j % 2 == 0) {
                        if( j >= 1 ) {
                            sumB += blueGr[(i / 2) * width / 2 + ( (j - 1) / 2)];
                        }
                        if( j + 1 < width ) {
                            sumB += blueGr[(i / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        if( i >= 1) {
                            sumR += red_Gr[((i - 1) / 2) * width / 2 + (j / 2)];
                        }
                        if( i + 1 < height ) {
                            sumR += red_Gr[((i + 1) / 2) * width / 2 + (j / 2)];
                        }
                        result[imOff + 0] = sumR / 2;
                        result[imOff + 1] = grn2Gr[(i / 2) * width / 2 + (j / 2)];
                        result[imOff + 2] = sumB / 2;

                    } else {
                        if( j >= 1 ) {
                            sumG += grn2Gr[(i / 2) * width / 2 + ((j - 1) / 2)];
                        }
                        if( j + 1 < width) {
                            sumG += grn2Gr[(i / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        if( i >= 1) {
                            sumG += grn1Gr[((i - 1) / 2) * width / 2 + (j / 2)];
                        }
                        if( i + 1 < height) {
                            sumG += grn1Gr[((i + 1) / 2) * width / 2 + (j / 2)];
                        }
                        if( i >=1 && j >= 1) {
                            sumR += red_Gr[((i - 1) / 2) * width / 2 + ((j - 1) / 2)];
                        }
                        if( i >= 1 && j + 1 < width) {
                            sumR += red_Gr[((i - 1) / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        if( i + 1 < height && j >= 1) {
                            sumR += red_Gr[((i + 1) / 2) * width / 2 + ((j - 1) / 2)];
                        }
                        if( i + 1 < height && j + 1 < width ) {
                            sumR += red_Gr[((i + 1) / 2) * width / 2 + ((j + 1) / 2)];
                        }
                        result[imOff + 0] = sumR / 4;
                        result[imOff + 1] = sumG / 4;
                        result[imOff + 2] = blueGr[(i / 2) * width / 2 + (j / 2)];

                    }
                }
            }
            break;
        default:
            break;
    }

    return lodepng::encode(filename, result, width, height, LCT_RGB);
}

char *Debayer::DebayerContainer::augmented(const char *prefix, Channel ch) {
    strcpy(tempstr, prefix);
    switch (ch) {
        case RED:
            strcat(tempstr, "red.png");
            break;
        case GR1:
            strcat(tempstr, "gr1.png");
            break;
        case GR2:
            strcat(tempstr, "gr2.png");
            break;
        case BLU:
            strcat(tempstr, "blu.png");
            break;
    }
    return tempstr;
}

Debayer::DebayerContainer::~DebayerContainer() {
    delete tempstr;
    delete iBuf;
}
