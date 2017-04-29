#ifndef TEXTURE_BINDER_H
#define TEXTURE_BINDER_H

#include <map>
#include <memory>
#include <stdint.h>
#include <string>

#include "colorado/gl.h"
#include "../ecs/ecs.h"

namespace Terf {
class Archive;
}

struct Texture {
	GLuint handle;
	
	Texture (const Terf::Archive & terf, const std::string &);
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
