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
#include <time.h>
#include <windows.h>

static const int FONT_SIZE[] = {512, 256, 128, 64, 32, 16, 8};
//static const Color colors[] = {Color(0, 204, 51), Color(0, 204, 102), Color(0, 255, 102), Color(0, 204, 0)};
static const Color colors[] = {Color(102, 0, 153), Color(102, 0, 102), Color(102, 0, 204), Color(102, 0, 255)};

using namespace std;
static int excTime = 0;
static int excTime2 = 0;

void makeRect(int x, int y, int rectWidth, int rectHeight, unsigned totalWidth, unsigned totalHeight, unsigned char* dest, unsigned char* resultImage, unsigned char* input)
{
	excTime++;
    Color color = colors[rand()%4];
    for (int i = 0; i < rectHeight; ++i)
    {
        for (int j = 0; j < rectWidth; ++j)
        {
            if (input[j + rectWidth * i] != 0)
            {
				int index = (x + j + totalWidth * (y + i)) * 4;
                resultImage[index] = color.r;
                resultImage[index + 1] = color.g;
                resultImage[index + 2] = color.b;
                resultImage[index + 3] = input[j + rectWidth * i];
                
                dest[index] = 0;
                dest[index + 1] = 255;
                dest[index + 2] = 0;
            }
        }
    }
}

bool canFill(int x, int y, int rectWidth, int rectHeight, unsigned totalWidth, unsigned totalHeight, unsigned char* dest, unsigned char* resultImage)
{
	excTime2++;
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
    
    unsigned char* image = nullptr; //the raw pixels
    unsigned char* resultImages = nullptr;
    
    //load and decode
    unsigned error = 0;

	Font::getInstance()->init();
	auto fontdataList = Font::getInstance()->genarateFontData();


	std::cout << "Hello, World!\n";
    if(!error) error = lodepng_decode32_file(&image, &width, &height, "resources/karate-flyingkick-icon.png");
	resultImages = (unsigned char*)malloc(width * height * 4);
	memset(resultImages, 0, sizeof(char) * width * height * 4);
	time_t startTime = time(NULL);
	time_t fontInitStart = time(NULL);

    

	DWORD t1, t2;
	t1 = GetTickCount();
    




// 	time_t fontInitStop = time(NULL);
// 	printf("Use Time:%ld\n", (fontInitStop - fontInitStart));




    
    //auto fontdataList = Font::getInstance()->genarateFontDataInangle(45);
    
    //Font::getInstance()->genarateFontDataInangle(45);
    


    for (int k = 0; k < sizeof(FONT_SIZE)/sizeof(int); ++k)
    {
		int step = max(1, FONT_SIZE[k] / 4);
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
            for (int i = 0; i < width; i+=step)
            {
                for (int j = 0; j < height; j += step)
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

	t2 = GetTickCount();
	printf("Use Time:%d\n", (t2 - t1));

	printf("make rect Time:%d\n", excTime);
	printf("canfill Time:%d\n", excTime2);
    
    lodepng::encode("resources/karate-flyingkick-icon6.png", resultImages, width, height);


    return 0;
}
