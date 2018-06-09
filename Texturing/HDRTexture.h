#ifndef HDR_TEXTURE_H
#define HDR_TEXTURE_H
#include <string>

class HDRTexture
{
public:
	HDRTexture(std::string path);
	~HDRTexture();
	unsigned int ID() { return id; }
	unsigned int Width() { return width; }
	unsigned int Height() { return height; }
private:
	unsigned int id;
	int width;
	int height;
};

#endif