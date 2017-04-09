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

void makeRect(int x, int y, int rectWidth, int rectHeight, unsigned totalWidth, unsigned totalHeight, unsigned char* mask, unsigned char* resultImage, unsigned char* input)
{
    Color color = colors[rand()%4];
    for (int i = 0; i < rectHeight; ++i)
    {
        for (int j = 0; j < rectWidth; ++j)
        {
            if (input[j + rectWidth * i] != 0)
            {
				if (input[j + rectWidth * i] != 1)
				{
					int index = (x + j + totalWidth * (y + i)) * 4;
					resultImage[index] = color.r;
					resultImage[index + 1] = color.g;
					resultImage[index + 2] = color.b;
					resultImage[index + 3] = input[j + rectWidth * i];
				}
				mask[(x + j + totalWidth * (y + i))] = 1;
            }
        }
    }
}

bool canFill(int x, int y, int rectWidth, int rectHeight, unsigned totalWidth, unsigned totalHeight, unsigned char* dest, unsigned char* resultImage, unsigned char* mask)
{
	int totalCount = totalWidth * totalHeight;
    for (int i = 0; i < rectHeight; ++i)
    {
        for (int j = 0; j < rectWidth; ++j)
        {
            if ((x + j + totalWidth * (y + i)) > totalCount)
            {
                return false;
            }
            
            if (dest[(x + j + totalWidth * (y + i)) * 4 + 3] == 0 || mask[x + j + totalWidth * (y + i)] == 1)
            {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, const char * argv[]) {    
    unsigned width = 0;
    unsigned height = 0;
    
    unsigned char* image = nullptr; //the raw pixels
    unsigned char* resultImages = nullptr;
    
    //load and decode
    unsigned error = 0;

	Font::getInstance()->init();
	auto fontdataList = Font::getInstance()->genarateFontData();
	auto fontdataList2 = Font::getInstance()->genarateFontDataInangle(90);
	for (auto i = fontdataList2.begin(); i != fontdataList2.end(); ++i)
	{
		for (auto j = i->second.begin(); j != i->second.end(); ++j)
		{
			fontdataList[i->first].push_back(*j);
		}
	}
	DWORD t1, t2;
	t1 = GetTickCount();
    if(!error) error = lodepng_decode32_file(&image, &width, &height, "resources/dog.png");
	resultImages = (unsigned char*)malloc(sizeof(char) * width * height * 4);
	memset(resultImages, 0, sizeof(char) * width * height * 4);

	unsigned char* mask = (unsigned char*)malloc(sizeof(char) * width * height);
	memset(mask, 0, sizeof(char) * width * height);
    for (int k = 0; k < sizeof(FONT_SIZE)/sizeof(int); ++k)
    {
		int step = max(1, FONT_SIZE[k] / 4);
        int index = 0;
//         if (k < 3)
//         {
//             for (int count = 0; count < 500; ++count)
//             {
//                 auto &fontData = fontdataList[FONT_SIZE[k]][index];
//                 int i = rand() % width;
//                 int j = rand() % height;
//                 if (canFill(i, j, fontData.width, fontData.height, width, height, image, resultImages, mask))
//                 {
//                     makeRect(i, j, fontData.width, fontData.height, width, height, mask, resultImages, fontData.data);
//                     index++;
//                     if (index >= fontdataList[FONT_SIZE[k]].size())
//                     {
//                         index = 0;
//                     }
//                 }
//             }
//         }
//         else
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    auto &fontData = fontdataList[FONT_SIZE[k]][index];
                    if (canFill(i, j, fontData.width, fontData.height, width, height, image, resultImages, mask))
                    {
                        makeRect(i, j, fontData.width, fontData.height, width, height, mask, resultImages, fontData.data);
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
    
    lodepng::encode("resources/karate-flyingkick-icon7.png", resultImages, width, height);
    return 0;
}
