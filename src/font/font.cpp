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
        if (FT_New_Face(ft, "resources/gongfang.otf", 0, &face))
        //if (FT_New_Face(ft, "mifu.ttf", 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        }
        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, FONT_SIZE[i]);
        faces.push_back(face);
    }
    
    // Destroy FreeType once we're finished
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

vector<unsigned char> Font::getCharInfo(int index, int &width, int &height, int fontSize)
{
    int fontIndex = getFontIndex(fontSize);
    
    if (fontIndex == -1)
    {
        return vector<unsigned char>();
    }
    
    // Load character glyph
    if (FT_Load_Char(faces[fontIndex], index, FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }
    
    width = faces[fontIndex]->glyph->bitmap.width;
    height = faces[fontIndex]->glyph->bitmap.rows;
    int left = faces[fontIndex]->glyph->bitmap_left;
    int top = (int)(-faces[fontIndex]->glyph->bitmap_top + (faces[fontIndex]->size->metrics.ascender >> 6));
    ;

    std::vector<unsigned char> buffers;
    
    for (int i = 0; i < fontSize; ++i)
    {
        for (int j = 0; j < fontSize; j++)
        {
            if (i < top || j < left || i >= top + height || j >= left + width)
            {
                buffers.push_back(0);
                buffers.push_back(0);
                buffers.push_back(0);
                buffers.push_back(0);
            }
            else
            {
                buffers.push_back(faces[fontIndex]->glyph->bitmap.buffer[(i - top) * width + j - left]);
                buffers.push_back(0);
                buffers.push_back(0);
                buffers.push_back(faces[fontIndex]->glyph->bitmap.buffer[(i - top) * width + j - left]);
            }
        }
    }
    
    width = fontSize;
    height = fontSize;
    
    return std::move(buffers);
}

std::vector<unsigned char> Font::getStringInfo(const std::string &text, int &width, int &height, int fontSize)
{
    std::vector<std::vector<unsigned char>> resultList;
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
        
        int charWidth = 0;
        int charHeight = 0;
        auto charInfo = getCharInfo(result, charWidth, charHeight, fontSize);
        resultList.push_back(charInfo);
        fontLength++;
    }
    
    std::vector<unsigned char> finalResult;
    for (int i = 0; i < fontSize; ++i)
    {
        for (int j = 0; j < fontSize * fontLength; ++j)
        {
            finalResult.push_back(resultList[j / fontSize][(i * fontSize + j % fontSize) * 4]);
            finalResult.push_back(resultList[j / fontSize][(i * fontSize + j % fontSize) * 4 + 1]);
            finalResult.push_back(resultList[j / fontSize][(i * fontSize + j % fontSize) * 4 + 2]);
            finalResult.push_back(resultList[j / fontSize][(i * fontSize + j % fontSize) * 4 + 3]);
        }
    }
    
    width = fontSize * fontLength;
    height = fontSize;
    return std::move(finalResult);
}

int Font::getCharIndex()
{
    return 0;
}

void Font::getFontData(std::string text, float x, float y, float scale)
{
    int len = 0;
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
    }
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

std::map<int, std::vector<FontData>> Font::genarateFontDataInangle(double angle)
{
    auto wordList = getStringList();
    
    std::map<int, std::vector<FontData>> fontDatas;
    for (int j = 0; j < sizeof(FONT_SIZE) / sizeof(int); ++j)
    {
        std::vector<FontData> singleWordList;
        for (int i = 0; i < wordList.size(); ++i)
        {
            FontData fontData;
            int width = 0;
            int height = 0;
            auto data = getStringInfo(wordList[i], width, height, FONT_SIZE[j]);
            fontData.width = width;
            fontData.height = height;
            fontData.data = data;
            
            for (int m = 0; m < width * height; ++m)
            {
				data.push_back(255);
				data.push_back(0);
				data.push_back(0);
				data.push_back(255);
            }

			int oldWidth = width;
			int oldHeight = height;
			double fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;
			fSrcX1 = (double)(-(oldWidth - 1) / 2);
			fSrcY1 = (double)((oldHeight - 1) / 2);
			fSrcX2 = (double)((oldWidth - 1) / 2);
			fSrcY2 = (double)((oldHeight - 1) / 2);
			fSrcX3 = (double)(-(oldWidth - 1) / 2);
			fSrcY3 = (double)(-(oldHeight - 1) / 2);
			fSrcX4 = (double)((oldWidth - 1) / 2);
			fSrcY4 = (double)(-(oldHeight - 1) / 2);

			double arcAngle = 3.1415926 / 180 * angle;

			double fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;
			fDstX1 = cos(arcAngle) * fSrcX1 + sin(arcAngle) * fSrcY1;
			fDstY1 = -sin(arcAngle) * fSrcX1 + cos(arcAngle) * fSrcY1;
			fDstX2 = cos(arcAngle) * fSrcX2 + sin(arcAngle) * fSrcY2;
			fDstY2 = -sin(arcAngle) * fSrcX2 + cos(arcAngle) * fSrcY2;
			fDstX3 = cos(arcAngle) * fSrcX3 + sin(arcAngle) * fSrcY3;
			fDstY3 = -sin(arcAngle) * fSrcX3 + cos(arcAngle) * fSrcY3;
			fDstX4 = cos(arcAngle) * fSrcX4 + sin(arcAngle) * fSrcY4;
			fDstY4 = -sin(arcAngle) * fSrcX4 + cos(arcAngle) * fSrcY4;
            
            lodepng::encode("resources/plain.png", data, width, height);

			int newWidth = (max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
			int newHeight = (max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);

			double dx = -0.5*newWidth*cos(arcAngle) - 0.5*newHeight*sin(arcAngle) + 0.5*oldWidth;
			double dy = 0.5*newWidth*sin(arcAngle) - 0.5*newHeight*cos(arcAngle) + 0.5*oldHeight;


			vector<unsigned char> finalResult;
			finalResult.resize(newWidth * newHeight * 4);
			int x, y;
			for (int i = 0; i < newHeight; i++)
			{
				for (int j = 0; j < newWidth; j++)
				{
					x = double(j)*cos(arcAngle) + double(i)*sin(arcAngle) + dx;
					y = double(-j)*sin(arcAngle) + double(i)*cos(arcAngle) + dy;

					if ((x < 0) || (x >= oldWidth) || (y < 0) || (y >= oldHeight))
					{
						finalResult[(i * newWidth + j) * 4] = 0;
						finalResult[(i * newWidth + j) * 4 + 1] = 0;
						finalResult[(i * newWidth + j) * 4 + 2] = 0;
						finalResult[(i * newWidth + j) * 4 + 3] = 0;
					}
					else
					{
						finalResult[(i * newWidth + j) * 4] = data[(y * oldWidth + x) * 4];
						finalResult[(i * newWidth + j) * 4 + 1] = data[(y * oldWidth + x) * 4 + 1];
						finalResult[(i * newWidth + j) * 4 + 2] = data[(y * oldWidth + x) * 4 + 2];
						finalResult[(i * newWidth + j) * 4 + 3] = data[(y * oldWidth + x) * 4 + 3];
					}
				}
			}
            
            lodepng::encode("resources/angle.png", finalResult, newWidth, newHeight);
            
            singleWordList.push_back(fontData);
        }
        fontDatas.insert(make_pair(FONT_SIZE[j], singleWordList));
    }
    
    return std::move(fontDatas);
}

std::map<int, std::vector<FontData>> Font::genarateFontData()
{
    auto wordList = getStringList();
    
    std::map<int, std::vector<FontData>> fontDatas;
    for (int j = 0; j < sizeof(FONT_SIZE) / sizeof(int); ++j)
    {
        std::vector<FontData> singleWordList;
        for (int i = 0; i < wordList.size(); ++i)
        {
            FontData fontData;
            int width = 0;
            int height = 0;
            auto data = getStringInfo(wordList[i], width, height, FONT_SIZE[j]);
            fontData.width = width;
            fontData.height = height;
            fontData.data = data;
            
            singleWordList.push_back(fontData);
        }
        fontDatas.insert(make_pair(FONT_SIZE[j], singleWordList));
    }
    
    return std::move(fontDatas);
}
