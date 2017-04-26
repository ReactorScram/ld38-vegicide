#include "texture-binder.h"

#include "colorado/image-to-texture.h"
#include "terf/terf.h"

using namespace Colorado;

Texture::Texture (const Terf::Archive & terf, const std::string & fn) :
	handle (0)
{
	auto buf = terf.lookupFile (fn);
	if (buf.size () > 0) {
		autoImageToTexture (buf, handle, 4);
	}
}

Texture::~Texture () {
	if (handle > 0) {
		glDeleteTextures (1, &handle);
	}
}

void Texture::bind () const {
	glBindTexture (GL_TEXTURE_2D, handle);
}

TextureBinder::TextureBinder () : m_current (nullptr), map () {}

void TextureBinder::add (TextureKey code, Texture * t) {
	map [code] = std::unique_ptr <Texture> (t);
}

const Texture * TextureBinder::current () const {
	return m_current;
}

void TextureBinder::bind (TextureKey code) {
	bind (get (code));
}

void TextureBinder::bind (const Texture * t) {
	m_current = t;
	m_current->bind ();
}

const Texture * TextureBinder::get (TextureKey code) const {
	const Texture * result = map.at (code).get ();
	if (result != nullptr) {
		return result;
	}
	else {
		// Throw error
		return result;
	}
}
