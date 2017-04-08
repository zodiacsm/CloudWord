//
//  main.cpp
//  CloudWord
//
//  Created by 施铭 on 17/3/30.
//  Copyright © 2017年 施铭. All rights reserved.
//

#include <iostream>
#include "SOIL.h"
#include <vector>
#include "lodepng.h"
#include "font.h"

static const int FONT_SIZE[] = {512, 256, 128, 64, 32, 16, 8};
//static const Color colors[] = {Color(0, 204, 51), Color(0, 204, 102), Color(0, 255, 102), Color(0, 204, 0)};
static const Color colors[] = {Color(102, 0, 153), Color(102, 0, 102), Color(102, 0, 204), Color(102, 0, 255)};

using namespace std;

void makeRect(int x, int y, int rectWidth, int rectHeight, unsigned totalWidth, unsigned totalHeight, std::vector<unsigned char> &dest, vector<unsigned char> &resultImage, vector<unsigned char> &input)
{
    Color color = colors[rand()%4];
    for (int i = 0; i < rectHeight; ++i)
    {
        for (int j = 0; j < rectWidth; ++j)
        {
            if (input[(j + rectWidth * i) * 4 + 3] != 0)
            {
                resultImage[(x + j + totalWidth * (y + i)) * 4] = color.r;
                resultImage[(x + j + totalWidth * (y + i)) * 4 + 1] = color.g;
                resultImage[(x + j + totalWidth * (y + i)) * 4 + 2] = color.b;
                resultImage[(x + j + totalWidth * (y + i)) * 4 + 3] = input[(j + rectWidth * i) * 4 + 3];
                
                dest[(x + j + totalWidth * (y + i)) * 4] = 0;
                dest[(x + j + totalWidth * (y + i)) * 4 + 1] = 255;
                dest[(x + j + totalWidth * (y + i)) * 4 + 2] = 0;
            }
        }
    }
}

bool canFill(int x, int y, int rectWidth, int rectHeight, unsigned totalWidth, unsigned totalHeight, std::vector<unsigned char> &dest, vector<unsigned char> &resultImage)
{
    for (int i = 0; i < rectHeight; ++i)
    {
        for (int j = 0; j < rectWidth; ++j)
        {
            if ((x + j + totalWidth * (y + i)) > totalWidth * totalHeight)
            {
                return false;
            }
            
            if (dest[(x + j + totalWidth * (y + i)) * 4 + 3] == 0 || (dest[(x + j + totalWidth * (y + i)) * 4] == 0 && dest[(x + j + totalWidth * (y + i)) * 4 + 1] == 255 && dest[(x + j + totalWidth * (y + i)) * 4 + 2] == 0))
            {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    
    unsigned width = 0;
    unsigned height = 0;
    
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; //the raw pixels
    std::vector<unsigned char> resultImages;
    
    //load and decode
    unsigned error = lodepng::load_file(png, "resources/karate-flyingkick-icon.png");
    
    if(!error) error = lodepng::decode(image, width, height, png);
    std::cout << image.size() << std::endl;
    for (int i = 0; i < image.size(); ++i) {
        resultImages.push_back(0);
    }
    
    Font::getInstance()->init();
    
    //auto fontdataList = Font::getInstance()->genarateFontData();
    
    auto fontdataList = Font::getInstance()->genarateFontDataInangle(45);
    
    //Font::getInstance()->genarateFontDataInangle(45);
    
    for (int k = 0; k < sizeof(FONT_SIZE)/sizeof(int); ++k)
    {
        int index = 0;
        if (k < 3)
        {
            for (int count = 0; count < 500; ++count)
            {
                auto &fontData = fontdataList[FONT_SIZE[k]][index];
                int i = rand() % width;
                int j = rand() % height;
                if (canFill(i, j, fontData.width, fontData.height, width, height, image, resultImages))
                {
                    makeRect(i, j, fontData.width, fontData.height, width, height, image, resultImages, fontData.data);
                    index++;
                    if (index >= fontdataList[FONT_SIZE[k]].size())
                    {
                        index = 0;
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < width; ++i)
            {
                for (int j = 0; j < height; ++j)
                {
                    auto &fontData = fontdataList[FONT_SIZE[k]][index];
                    if (canFill(i, j, fontData.width, fontData.height, width, height, image, resultImages))
                    {
                        makeRect(i, j, fontData.width, fontData.height, width, height, image, resultImages, fontData.data);
                        index++;
                        if (index >= fontdataList[FONT_SIZE[k]].size())
                        {
                            index = 0;
                        }
                    }
                }
            }
        }
    }
    
    lodepng::encode("resources/karate-flyingkick-icon3.png", resultImages, width, height);
    
    return 0;
}
