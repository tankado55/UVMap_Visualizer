#pragma once

class DepthTexture
{
private:
	unsigned int m_RendererID;
public:
	DepthTexture();
	~DepthTexture();

	unsigned int getID() const;
	void Bind(unsigned int slot = 0) const;
};