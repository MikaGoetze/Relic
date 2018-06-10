#ifndef RENDER_UTIL_RELIC_H
#define RENDER_UTIL_RELIC_H
#include <glm/mat4x4.hpp>
#include <Lighting/Shader.h>
#include <Lighting/PointLight.h>
#include <Lighting/SpotLight.h>
#include <Lighting/DirectionalLight.h>

class RenderUtil
{
public:
	struct DepthRet
	{
		unsigned int depth_map;
		glm::mat4 light_space;
	};
	static unsigned int RenderEquirectangularEnvironmentToCubemap(unsigned int id);
	static unsigned int ConvoluteCubemap(unsigned int id);
	static unsigned int PreFilterEnvironment(unsigned int id);
	static unsigned int CalculateBRDF_LUT();
	static DepthRet GetLightDepthMap(DirectionalLight* light);
	static DepthRet GetLightDepthMap(SpotLight* light);
	static DepthRet GetLightDepthMap(PointLight* light);
	static void SetDirLightDM();
	static void SetSpotLightDM();
	static void SetPointLightDMs();
	static void RenderCube();
	static void RenderQuad();
	static void InitLightDepthMaps(Shader* shader);


private:
	static int RenderTextureFromCube(Shader* shader, unsigned int id, int size, bool cubemap_input, bool mipmaps);
	static bool has_buffered_cube;
	static unsigned int CubeVAO;
	static unsigned int CubeVBO;
	static unsigned int QuadVAO;
	static unsigned int QuadVBO;
	static unsigned int RBO;
	static unsigned int FBO;
	static unsigned int DFBO;
	static unsigned int d_depthMap;
	static unsigned int s_depthMap;
	static unsigned int p_lights[Light::MAX_P_LIGHTS];

	static Shader* s_light_shader;
	static Shader* d_light_shader;
};

#endif