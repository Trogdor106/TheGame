#include "ITexture.h"

void ITexture::Bind(uint32_t slot) {
	glBindTextureUnit(slot, myRendererID);
}

