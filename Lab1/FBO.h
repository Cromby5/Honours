#pragma once

#include <GL/glew.h>
#include "ShaderHandler.h"

class FBO
{
	public:
		FBO();
		~FBO();

		void genFBO(float w, float h);
		void bindFBO();
		void unbindFBO();

		void initQuad();
		void drawQuad();

		// Gbuffer, deffered rendering
		void GenGBuffer(GLsizei SCR_WIDTH, GLsizei SCR_HEIGHT);
		void geoPass();
		void lightPass();


	private:
		GLuint fbo; // Frame Buffer 
		GLuint rbo; // Render Buffer 
		GLuint cbo; // Colour Buffer 
		GLuint quadVAO;
		GLuint quadVBO;

		ShaderHandler fboShader;

		ShaderHandler gBufferShader;
		unsigned int gBuffer, gPosition, gNormal, gAlbedoSpec;
};
