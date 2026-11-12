#include "Texture.h"
#include <cmath>

std::vector<unsigned char> makeSwirlImage(int size) {
    std::vector<unsigned char> img(size * size * 3);
    for (int row = 0; row < size; row++)
        for (int col = 0; col < size; col++) {
            float x = (float)col / (size - 1), y = (float)row / (size - 1);
            float swirl = std::sin(10 * x + 3 * std::sin(8 * y)) + std::sin(10 * y + 3 * std::cos(7 * x));
            float ring = std::sin(25 * std::hypot(x - 0.5f, y - 0.5f));
            float t = 0.5f + 0.25f * swirl + 0.25f * ring;
            const float shift[3] = {0.0f, 0.33f, 0.66f};
            for (int k = 0; k < 3; k++)
                img[(row * size + col) * 3 + k] =
                    (unsigned char)(255 * (0.5f + 0.5f * std::sin(6.28f * (t + shift[k]))));
        }
    return img;
}

GLuint createTexture2D(const std::vector<unsigned char>& rgb, int size, int unit) {
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}
