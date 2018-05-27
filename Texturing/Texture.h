#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>

class Texture
{
public:
	Texture();
	Texture(std::string path, unsigned int source_type);
	~Texture();
	unsigned int ID() { return id; }
	unsigned int Width() { return width; }
	unsigned int Height() { return height; }
private:
	unsigned int id;
	int width;
	int height;
};

#endif