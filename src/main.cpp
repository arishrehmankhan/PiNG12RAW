/* Copyright (C) 2018 Supragya Raj
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * PiNG12RAW - (https://github.com/supragya/PiNG12RAW)
 * 
 * This project was originally develeped by Supragya Raj.
 * 
 * As a challenge Arish Rehman Khan added Bilinear debayering algorithm.
 *
 */

#include "Debayer/PiNG12RAW.h"

const char INPUT_FILE[] = "test_image/test.raw12";
const char RESULT_CHANNELS[] = "results/Channel_";
const char LINEAR_RESULT[] = "results/LinearDebayer.png";
const char BILINEAR_RESULT[] = "results/BilinearDebayer.png";

const int WIDTH = 4096;
const int HEIGHT = 3072;

int main() {

    char choice = '0';

    std::cout << "Which method of Debayering do you want to use ?" << std::endl;
    std::cout << "1. LINEAR" << std::endl;
    std::cout << "2. BILINEAR" << std::endl;
    std::cin >> choice ;

    if( choice == '1' || choice == '2' ) {

    std::cout << "{PiNG12RAW working! - Supragya Raj}" << std::endl;

    std::cout << "- Reading file: " << INPUT_FILE << std::endl;
    Debayer::DebayerContainer container(INPUT_FILE, WIDTH, HEIGHT);

    std::cout << "- Extracting Bayer Channels" << std::endl;
    container.ComputeChannels();

    std::cout << "- Writing Bayer Channels: " << RESULT_CHANNELS << "<cnl>.png" << std::endl;
    container.WriteChannels(RESULT_CHANNELS);

    if( choice == '1') {

        std::cout << "- Debayering LINEAR, image: " << LINEAR_RESULT << std::endl;
        container.WriteColored(LINEAR_RESULT, Debayer::DebayeringAlgorithm::LINEAR);

    } else if( choice == '2') {

        std::cout << "- Debayering BILINEAR, image: " << BILINEAR_RESULT << std::endl;
        container.WriteColored(BILINEAR_RESULT, Debayer::DebayeringAlgorithm::BILINEAR);

    }

    std::cout << "[All operations done]" << std::endl;

    } else {

        std::cout<<"Wrong choice entered! Exiting...";

    }

    return 0;
}
