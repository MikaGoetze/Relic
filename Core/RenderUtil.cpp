#include "RenderUtil.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Lighting/Shader.h"
#include "Core/Relic.h"
#include <Lighting/DirectionalLight.h>
#include <Lighting/SpotLight.h>
#include <Lighting/PointLight.h>
#include <glm/gtc/matrix_transform.inl>

unsigned int RenderUtil::RenderEquirectangularEnvironmentToCubemap(unsigned int id)
{
	Shader* render_shader = new Shader("Lighting/Shaders/cubemap_render.vert", "Lighting/Shaders/cubemap_render.frag");
	unsigned int tex = RenderTextureFromCube(render_shader, id, 512, false, false);
	delete render_shader;
	return tex;
}

unsigned RenderUtil::ConvoluteCubemap(unsigned id)
{
	Shader* render_shader = new Shader("Lighting/Shaders/cubemap_convolute.vert", "Lighting/Shaders/cubemap_convolute.frag");
	unsigned int tex = RenderTextureFromCube(render_shader, id, 32, true, false);
	delete render_shader;
	return tex;
}

unsigned RenderUtil::PreFilterEnvironment(unsigned id)
{
	Shader* render_shader = new Shader("Lighting/Shaders/cubemap_prefilter.vert", "Lighting/Shaders/cubemap_prefilter.frag");
	unsigned int tex = RenderTextureFromCube(render_shader, id, 128, true, true);
	delete render_shader;
	return tex;
}

unsigned RenderUtil::CalculateBRDF_LUT()
{
	Shader* shader = new Shader("Lighting/Shaders/brdf_lut.vert", "Lighting/Shaders/brdf_lut.frag");
	unsigned int lut;
	glGenTextures(1, &lut);
	if(FBO == 0 || RBO == 0)
	{
		glGenFramebuffers(1, &FBO);
		glGenRenderbuffers(1, &RBO);
	}
	glBindTexture(GL_TEXTURE_2D, lut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lut, 0);

	glViewport(0, 0, 512, 512);
	shader->SetActive();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderQuad();

	//Reset everything back to how it was
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, Relic::GetWindow()->WindowWidth(), Relic::GetWindow()->WindowHeight());
	
	Relic::GetStandardShader()->SetActive();

	return lut;
}

RenderUtil::DepthRet RenderUtil::GetLightDepthMap(DirectionalLight* light)
{
	glCullFace(GL_FRONT);
	if(DFBO == 0)
	{
		glGenFramebuffers(1, &DFBO);
	}

	unsigned int shadow_size = 4096;

	if(d_depthMap == 0)
		glGenTextures(1, &d_depthMap);
	glBindTexture(GL_TEXTURE_2D, d_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = { 1,1,1,1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	glBindFramebuffer(GL_FRAMEBUFFER, DFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	glViewport(0, 0, shadow_size, shadow_size);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 dir_light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 40.0f);
	glm::vec3 pos = -10.0f * light->GetDirection();
	glm::mat4 light_view = glm::lookAt(pos, pos + light->GetDirection(), glm::vec3(0, 1, 0));
	glm::mat4 light_space = dir_light_proj * light_view;

	if(d_light_shader == NULL) d_light_shader = new Shader("Lighting/Shaders/shadows.vert", "Lighting/Shaders/shadows.frag");
	d_light_shader->SetActive();
	d_light_shader->SetMat4("light_space", light_space);

	//Lets render the scene 
	Relic::GetCurrentScene()->Render(d_light_shader, dir_light_proj, light_view, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Relic::GetStandardShader()->SetActive();
	glViewport(0, 0, Relic::GetWindow()->WindowWidth(), Relic::GetWindow()->WindowHeight());
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DepthRet ret;
	ret.depth_map = d_depthMap;
	ret.light_space = light_space;
	return ret;
}

RenderUtil::DepthRet RenderUtil::GetLightDepthMap(SpotLight* light)
{
	glCullFace(GL_FRONT);
	if(DFBO == 0)
	{
		glGenFramebuffers(1, &DFBO);
	}

	unsigned int shadow_size = 4096;

	if(s_depthMap == 0)
		glGenTextures(1, &s_depthMap);

	glBindTexture(GL_TEXTURE_2D, s_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = { 1,1,1,1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	glBindFramebuffer(GL_FRAMEBUFFER, DFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	glViewport(0, 0, shadow_size, shadow_size);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 dir_light_proj = glm::perspective(45.0f, 1.0f, 0.1f, 40.0f);
	glm::vec3 pos = light->GetGameObject()->GetComponent<Transform>()->GetPosition();
	glm::mat4 light_view = glm::lookAt(pos, pos + light->GetDirection(), glm::vec3(0, 1, 0));
	glm::mat4 light_space = dir_light_proj * light_view;

	if (s_light_shader == NULL) s_light_shader = new Shader("Lighting/Shaders/shadows.vert", "Lighting/Shaders/shadows.frag");
	s_light_shader->SetActive();
	s_light_shader->SetMat4("light_space", light_space);

	//Lets render the scene 
	Relic::GetCurrentScene()->Render(s_light_shader, dir_light_proj, light_view, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Relic::GetStandardShader()->SetActive();
	glViewport(0, 0, Relic::GetWindow()->WindowWidth(), Relic::GetWindow()->WindowHeight());
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DepthRet ret;
	ret.depth_map = s_depthMap;
	ret.light_space = light_space;
	return ret;
}

RenderUtil::DepthRet RenderUtil::GetLightDepthMap(PointLight* light)
{
	int index = light->GetIndex();
	if(p_lights[index] == 0)
	{
		glGenTextures(1, &p_lights[index]);
	}

	const unsigned int shadow_size = 4096;
	glBindTexture(GL_TEXTURE_CUBE_MAP, p_lights[index]);
	for(int i = 0; i < 6; i ++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if(DFBO == 0)
	{
		glGenFramebuffers(1, &DFBO);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, DFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, p_lights[index], 0); 
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, shadow_size, shadow_size);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::vec3 light_pos = light->GetGameObject()->GetComponent<Transform>()->GetPosition();

	glm::mat4 render_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light->GetFar());
	for (unsigned int i = 0; i < 6; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, p_lights[index], 0);
	}
	glm::mat4 render_view_mats[] =
	{
		render_projection * glm::lookAt(light_pos, light_pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
		render_projection * glm::lookAt(light_pos, light_pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
		render_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
		render_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
		render_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
		render_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))
	};

	Shader shadows = Shader("Lighting/Shaders/point_shadows.vert", "Lighting/Shaders/point_shadows.frag", "Lighting/Shaders/point_shadows.geom");
	shadows.SetActive();

	shadows.SetMat4("view_mats[0]", render_view_mats[0]);
	shadows.SetMat4("view_mats[1]", render_view_mats[1]);
	shadows.SetMat4("view_mats[2]", render_view_mats[2]);
	shadows.SetMat4("view_mats[3]", render_view_mats[3]);
	shadows.SetMat4("view_mats[4]", render_view_mats[4]);
	shadows.SetMat4("view_mats[5]", render_view_mats[5]);

	shadows.SetVec3("light_position", light_pos);
	shadows.SetFloat("clipping_plane_far", light->GetFar());

	Relic::GetCurrentScene()->Render(&shadows, render_projection, glm::mat4(), true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Relic::GetStandardShader()->SetActive();
	glViewport(0, 0, Relic::GetWindow()->WindowWidth(), Relic::GetWindow()->WindowHeight());
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DepthRet ret;
	ret.depth_map = p_lights[index];
	ret.light_space = glm::mat4();
	return ret;
}

void RenderUtil::SetDirLightDM()
{
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, d_depthMap);
}

void RenderUtil::SetSpotLightDM()
{
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, s_depthMap);
}

void RenderUtil::SetPointLightDMs()
{
	for(int i = 0; i < Light::MAX_P_LIGHTS; i++)
	{
		glActiveTexture(GL_TEXTURE8 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, p_lights[i]);
	}
}

int RenderUtil::RenderTextureFromCube(Shader* render_shader, unsigned id, int size, bool cubemap_input, bool mipmaps)
{
	if(FBO == 0 || RBO == 0)
	{
		glGenFramebuffers(1, &FBO);
		glGenRenderbuffers(1, &RBO);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	unsigned int cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	//Generate all 6 faces
	for(unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (mipmaps)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//Finally lets render the actual cube

	glm::mat4 render_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 render_view_mats[] =
	{
		glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
		glm::lookAt(glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
		glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
		glm::lookAt(glm::vec3(0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
		glm::lookAt(glm::vec3(0), glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
		glm::lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))
	};

	//Bind the correct textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(cubemap_input ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, id);

	//Set up shader parameters
	render_shader->SetActive();
	render_shader->SetInt("environment_map", 0);
	render_shader->SetMat4("projection", render_projection);

	glViewport(0, 0, size, size);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	if(mipmaps)
	{
		for(int mip = 0; mip < 5; mip++)
		{
			int mip_width = size * std::pow(0.5, mip);
			int mip_height = mip_width;

			//resize render buffer to appropriate size for mip map
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
			glViewport(0, 0, mip_width, mip_height);

			float roughness = (float)mip / (float)(4);
			render_shader->SetFloat("roughness", roughness);

			//Finally lets render everything.
			for(unsigned int i = 0; i < 6; i++)
			{
				render_shader->SetMat4("view", render_view_mats[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				RenderCube();
			}	
		}
	}
	else
	{
		//Finally lets render everything.
		for(unsigned int i = 0; i < 6; i++)
		{
			render_shader->SetMat4("view", render_view_mats[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderCube();
			glfwSwapBuffers(Relic::GetWindow()->InternalWindow());
		}	
	}

	//return to our original framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Restore the viewport & shader
	glViewport(0, 0, Relic::GetWindow()->WindowWidth(), Relic::GetWindow()->WindowHeight());
	Relic::GetStandardShader()->SetActive();

	return cubemap;
}

void RenderUtil::RenderCube()
{
	if(!has_buffered_cube)
	{
		unsigned int VAO;
		unsigned int VBO;

		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(CubeVAO);

		CubeVAO = VAO;
		CubeVBO = VBO;
		has_buffered_cube = true;
	}
	
	glBindVertexArray(CubeVAO);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

void RenderUtil::RenderQuad()
{
	if (QuadVAO == 0)
	{
		float verts[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &QuadVAO);
		glBindVertexArray(QuadVAO);

		glGenBuffers(1, &QuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(QuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//Reset
	glBindVertexArray(0);
}

void RenderUtil::InitLightDepthMaps(Shader* shader)
{
	shader->SetInt("sun.depth_map", 3);
	shader->SetInt("pointLights[0].depth_map", 1);
	shader->SetInt("pointLights[1].depth_map", 1);
	shader->SetInt("pointLights[2].depth_map", 1);
	shader->SetInt("pointLights[3].depth_map", 1);
	shader->SetInt("spotLight.depth_map", 3);

}

unsigned int RenderUtil::CubeVBO;
unsigned int RenderUtil::CubeVAO;
unsigned int RenderUtil::QuadVAO;
unsigned int RenderUtil::QuadVBO;
unsigned int RenderUtil::RBO; 
unsigned int RenderUtil::d_depthMap;
unsigned int RenderUtil::s_depthMap;
unsigned int RenderUtil::FBO;
unsigned int RenderUtil::DFBO;
unsigned int RenderUtil::p_lights[Light::MAX_P_LIGHTS];
bool RenderUtil::has_buffered_cube;
Shader* RenderUtil::d_light_shader;
Shader* RenderUtil::s_light_shader;