#include <glm/glm.hpp>
#include <gl/glew.h>

#include "DepthMapFB.h"


DepthMapFB::DepthMapFB()
{
	glGenFramebuffers(1, &m_RendererID);
}

void DepthMapFB::attachTexture(const DepthTexture& tex)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.getID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
}

void DepthMapFB::bind() const
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

void DepthMapFB::unBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthMapFB::clear()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}
