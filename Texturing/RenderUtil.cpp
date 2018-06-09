#include "RenderUtil.h"
#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>
#include "Lighting/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Relic.h"

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

unsigned int RenderUtil::CubeVBO;
unsigned int RenderUtil::CubeVAO;
unsigned int RenderUtil::QuadVAO;
unsigned int RenderUtil::QuadVBO;
unsigned int RenderUtil::RBO; 
unsigned int RenderUtil::FBO;
bool RenderUtil::has_buffered_cube;