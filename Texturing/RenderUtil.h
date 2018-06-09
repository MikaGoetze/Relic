#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H
#include "Texture.h"

class Shader;

class RenderUtil
{
public:
	static unsigned int RenderEquirectangularEnvironmentToCubemap(unsigned int id);
	static unsigned int ConvoluteCubemap(unsigned int id);
	static unsigned int PreFilterEnvironment(unsigned int id);
	static unsigned int CalculateBRDF_LUT();
	static void RenderCube();
	static void RenderQuad();

private:
	static int RenderTextureFromCube(Shader* shader, unsigned int id, int size, bool cubemap_input, bool mipmaps);
	static bool has_buffered_cube;
	static unsigned int CubeVAO;
	static unsigned int CubeVBO;
	static unsigned int QuadVAO;
	static unsigned int QuadVBO;
	static unsigned int RBO;
	static unsigned int FBO;
};

#endif