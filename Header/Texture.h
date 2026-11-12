#pragma once
#include <glad/glad.h>
#include <vector>

std::vector<unsigned char> makeSwirlImage(int size);

GLuint createTexture2D(const std::vector<unsigned char>& rgb, int size, int unit = 0);
