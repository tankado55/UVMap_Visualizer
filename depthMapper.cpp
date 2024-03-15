#include <glm/glm.hpp>
#include <gl/glew.h>

#include "depthMapper.h"


DepthMapper::DepthMapper()
{
	glGenFramebuffers(1, &m_RendererID);
}
