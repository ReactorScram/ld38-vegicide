#ifndef TEXTURE_BINDER_H
#define TEXTURE_BINDER_H

#include <map>
#include <memory>
#include <stdint.h>

#include "colorado/gl.h"

typedef int64_t TextureKey;

namespace Terf {
class Archive;
}

struct Texture {
	GLuint handle;
	
	Texture (const Terf::Archive & terf, std::string);
	~Texture ();
	
	void bind () const;
};

struct TextureBinder {
	TextureBinder ();
	
	TextureBinder (const TextureBinder &) = delete;
	TextureBinder & operator = (const TextureBinder &) = delete;
	
	void add (TextureKey, Texture *);
	const Texture * current () const;
	void bind (TextureKey);
	
private:
	const Texture * m_current;
	std::map <TextureKey, std::unique_ptr <const Texture> > map;
	
	void bind (const Texture *);
	const Texture * get (TextureKey) const;
};

#endif
