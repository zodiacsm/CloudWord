//
//  font.h
//  GlewTest
//
//  Created by 施铭 on 16/11/2.
//  Copyright © 2016年 施铭. All rights reserved.
//

#ifndef FONT_H
#define FONT_H

#include <freetype/ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <vector>
#include <map>

class Color
{
public:
    Color(int r, int g, int b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    
    int r;
    int g;
    int b;
};

class FontData
{
public:
    int width;
    int height;
    std::vector<unsigned char> data;
};

class Font
{
private:
    Font();
    
public:
    static Font* getInstance();

public:
    void init();
    
    std::vector<unsigned char> getCharInfo(int index, int &width, int &height, int fontSize);
    
    std::vector<unsigned char> getStringInfo(const std::string &text, int &width, int &height, int fontSize);
    
    std::vector<std::string> getStringList();
    
    std::map<int, std::vector<FontData>> genarateFontData();
    
    std::map<int, std::vector<FontData>> genarateFontDataInangle(double angle);
    
    int getCharIndex();
    
    void getFontData(std::string text, float x, float y, float scale);
    
    int getFontIndex(int fontSize);
    
    static Font* mFont;
    
    FT_Library ft;
    std::vector<FT_Face> faces;
};




#endif /* font_h */
