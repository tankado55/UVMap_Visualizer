#pragma once

#include "DepthTexture.h"


const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class DepthMapFB
{
private:
	unsigned int m_RendererID;

public:
	DepthMapFB();
	void attachTexture(const DepthTexture& tex);
	void bind() const;
	void unBind() const;
	void clear();
};