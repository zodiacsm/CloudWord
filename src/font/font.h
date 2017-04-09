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

class FontDataBuffer
{
public:
	int width;
	int height;
	unsigned char* data;

	FontDataBuffer(unsigned char* data = nullptr, int width = 0, int height = 0)
		:data(data), width(width), height(height)
	{
	}
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

	FontDataBuffer getCharInfo(int index, int fontSize);
    
	FontDataBuffer getStringInfo(const std::string &text, int fontSize);
    
    std::vector<std::string> getStringList();
    
    std::map<int, std::vector<FontDataBuffer>> genarateFontData();
    
    std::map<int, std::vector<FontDataBuffer>> genarateFontDataInangle(double angle);
    
    void getFontData(std::string text, float x, float y, float scale);
    
    int getFontIndex(int fontSize);
    
    static Font* mFont;
    
    FT_Library ft;
    std::vector<FT_Face> faces;
};




#endif /* font_h */
