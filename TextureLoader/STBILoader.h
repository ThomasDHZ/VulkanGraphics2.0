#pragma once
#include "TextureInfo.h"
#include <string>

class STBILoader
{
private:
public:
    STBILoader();
    ~STBILoader();
    TextureInfo LoadSTBILoader(const std::string& TextureLocation);
};

