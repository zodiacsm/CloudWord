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
            
            std::vector<unsigned char> plainData;
            width = 4;
            height = 4;
            
            for (int m = 0; m < width * height; ++m)
            {
                plainData.push_back(255);
                plainData.push_back(0);
                plainData.push_back(0);
                plainData.push_back(255);
            }
            
            lodepng::encode("resources/plain.png", plainData, width, height);
            
            
            lodepng::encode("resources/angle.png", finalResult, width * cos(angleArc) + height * sin(angleArc),
                            width * sin(angleArc) + height * cos(angleArc));
            
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
