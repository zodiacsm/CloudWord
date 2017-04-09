//
//  font.cpp
//  GlewTest
//
//  Created by 施铭 on 16/11/2.
//  Copyright © 2016年 施铭. All rights reserved.
//

#include <stdio.h>
#include "font.h"
#include <iostream>
#include <freetype/ft2build.h>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "lodepng.h"
#include <time.h>

Font* Font::mFont = nullptr;

static const int FONT_SIZE[] = {512, 256, 128, 64, 32, 16, 8};

static const std::string WORD_PATH = "resources/word.txt";

using namespace std;

Font::Font()
{
    
}

Font* Font::getInstance()
{
    if (!mFont)
    {
        mFont = new Font();
    }
    return mFont;
}

void Font::init()
{
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    
    for (int i = 0; i < sizeof(FONT_SIZE) / sizeof(int); ++i)
    {
        FT_Face face;
        if (FT_New_Face(ft, "resources/huakang.ttf", 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        }
        FT_Set_Pixel_Sizes(face, 0, FONT_SIZE[i]);
        faces.push_back(face);
    }

//    FT_Done_Face(face);
//    FT_Done_FreeType(ft);
}

int Font::getFontIndex(int fontSize)
{
    for (int i = 0; i < sizeof(FONT_SIZE) / sizeof(int); ++i)
    {
        if (FONT_SIZE[i] <= fontSize)
        {
            return i;
        }
    }
    return -1;
}

FontDataBuffer Font::getCharInfo(int index, int fontSize)
{
	int fontIndex = getFontIndex(fontSize);

	if (fontIndex == -1)
	{
		return FontDataBuffer(nullptr, 0, 0);
	}

	// Load character glyph
	if (FT_Load_Char(faces[fontIndex], index, FT_LOAD_RENDER))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	}

	int charWidth = faces[fontIndex]->glyph->bitmap.width;
	int charHeight = faces[fontIndex]->glyph->bitmap.rows;
	int left = faces[fontIndex]->glyph->bitmap_left;

	unsigned char* buffers = (unsigned char*)malloc(sizeof(char) * fontSize * fontSize);
	memset(buffers, 0, sizeof(char) * fontSize * fontSize);

	for (int i = 0; i < charHeight; ++i)
	{
		memcpy(buffers + i * fontSize + left, faces[fontIndex]->glyph->bitmap.buffer + i * charWidth, charWidth);
	}


// 	cout << charHeight * fontSize + charWidth << endl;
// 	cout << fontSize * fontSize << endl;
// 	memset(buffers + left, 1, sizeof(char) * charWidth);
// 	memset(buffers + (charHeight + 1) * fontSize + left, 1, sizeof(char) * charWidth);

	FontDataBuffer fontDataBuffer;
	fontDataBuffer.data = buffers;
	fontDataBuffer.width = fontSize;
	fontDataBuffer.height = fontSize;

	return fontDataBuffer;
}

FontDataBuffer Font::getStringInfo(const std::string &text, int fontSize)
{
    std::vector<FontDataBuffer> resultList;
    int len = 0;
    int fontLength = 0;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c+=len)
    {
        int result = 0;
        if ((*c & 0x80) == 0x0)
        {
            len = 1;
            result = *c;
        }
        else if ((*c & 0xE0) == 0xB0)
        {
            len = 2;
        }
        else if ((*c & 0xF0) == 0xE0)
        {
            len = 3;
            int m = (0x0F & *c);
            int n = (0x3F & *(c + 1));
            int k = (0x3F & *(c + 2));
            
            result = m << 12 | n << 6 | k;
        }
        
        auto charInfo = getCharInfo(result, fontSize);
        resultList.push_back(charInfo);
        fontLength++;
    }
    
	unsigned char* finalResult = (unsigned char*)malloc(fontSize * fontSize * fontLength);
	memset(finalResult, 0, sizeof(char) * fontSize * fontSize * fontLength);
    for (int i = 0; i < fontSize; ++i)
    {
        for (int j = 0; j < fontLength; ++j)
        {
			memcpy(finalResult + i * fontLength * fontSize + j * fontSize, resultList[j].data + i * fontSize, fontSize);
        }
    }
    
	FontDataBuffer fontDataBuffer;
	fontDataBuffer.data = finalResult;
	fontDataBuffer.width = fontSize * fontLength;
	fontDataBuffer.height = fontSize;
    return std::move(fontDataBuffer);
}

std::vector<std::string> Font::getStringList()
{
    std::ifstream infile;
    infile.open(WORD_PATH);
    
    std::vector<std::string> wordList;
    string s;
    while(getline(infile,s))
    {
        wordList.push_back(s);
    }
    infile.close();
    
    return std::move(wordList);
}

std::map<int, std::vector<FontDataBuffer>> Font::genarateFontDataInangle(double angle)
{
    auto wordList = getStringList();

	double arcAngle = 3.1415926 / 180 * angle;
	double sinAngle = sin(arcAngle);
	double cosAngle = cos(arcAngle);
    
    std::map<int, std::vector<FontDataBuffer>> fontDatas;
    for (int j = 0; j < sizeof(FONT_SIZE) / sizeof(int); ++j)
    {
        std::vector<FontDataBuffer> singleWordList;
        for (int i = 0; i < wordList.size(); ++i)
        {
			FontDataBuffer fontData = getStringInfo(wordList[i], FONT_SIZE[j]);

			int oldWidth = fontData.width;
			int oldHeight = fontData.height;
			double fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;
			fSrcX1 = (double)(-(oldWidth - 1) / 2);
			fSrcY1 = (double)((oldHeight - 1) / 2);
			fSrcX2 = (double)((oldWidth - 1) / 2);
			fSrcY2 = (double)((oldHeight - 1) / 2);
			fSrcX3 = (double)(-(oldWidth - 1) / 2);
			fSrcY3 = (double)(-(oldHeight - 1) / 2);
			fSrcX4 = (double)((oldWidth - 1) / 2);
			fSrcY4 = (double)(-(oldHeight - 1) / 2);

			double fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;
			fDstX1 = cosAngle * fSrcX1 + sinAngle * fSrcY1;
			fDstY1 = -sinAngle * fSrcX1 + cosAngle * fSrcY1;
			fDstX2 = cosAngle * fSrcX2 + sinAngle * fSrcY2;
			fDstY2 = -sinAngle * fSrcX2 + cosAngle * fSrcY2;
			fDstX3 = cosAngle * fSrcX3 + sinAngle * fSrcY3;
			fDstY3 = -sinAngle * fSrcX3 + cosAngle * fSrcY3;
			fDstX4 = cosAngle * fSrcX4 + sinAngle * fSrcY4;
			fDstY4 = -sinAngle * fSrcX4 + cosAngle * fSrcY4;

			int newWidth = (max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
			int newHeight = (max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);

			double dx = -0.5*newWidth*cosAngle - 0.5*newHeight*sinAngle + 0.5*oldWidth;
			double dy = 0.5*newWidth*sinAngle - 0.5*newHeight*cosAngle + 0.5*oldHeight;

			unsigned char* finalData = (unsigned char*)malloc(newWidth * newHeight);
			memset(finalData, 0, sizeof(char) * newWidth * newHeight);
			
			int x, y;
			for (int i = 0; i < newHeight; i++)
			{
				for (int j = 0; j < newWidth; j++)
				{
					x = double(j)*cosAngle + double(i)*sinAngle + dx;
					y = double(-j)*sinAngle + double(i)*cosAngle + dy;

					if (!((x < 0) || (x >= oldWidth) || (y < 0) || (y >= oldHeight)))
					{
						finalData[i * newWidth + j] = fontData.data[y * oldWidth + x];
					}
				}
			}
            
			FontDataBuffer finalDataBuffer;
			finalDataBuffer.width = newWidth;
			finalDataBuffer.height = newHeight;
			finalDataBuffer.data = finalData;
            singleWordList.push_back(finalDataBuffer);
//             if (FONT_SIZE[j] == 32 || FONT_SIZE[j] == 16)
//             {
//                 fontData.width = newWidth;
//                 fontData.height = newHeight;
//                 fontData.data = finalResult;
//                 singleWordList.push_back(fontData);
//             }
        }
        fontDatas.insert(make_pair(FONT_SIZE[j], singleWordList));
    }
    
    return std::move(fontDatas);
}

std::map<int, std::vector<FontDataBuffer>> Font::genarateFontData()
{
	auto wordList = getStringList();

	std::map<int, std::vector<FontDataBuffer>> fontDatas;
	for (int j = 0; j < sizeof(FONT_SIZE) / sizeof(int); ++j)
	{
		std::vector<FontDataBuffer> singleWordList;
		for (int i = 0; i < wordList.size(); ++i)
		{
			FontDataBuffer fontData = getStringInfo(wordList[i], FONT_SIZE[j]);

			singleWordList.push_back(fontData);
		}
		fontDatas.insert(make_pair(FONT_SIZE[j], singleWordList));
	}

	return std::move(fontDatas);
}
