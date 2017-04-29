#ifndef SHADERBINDER_H
#define SHADERBINDER_H

#include <memory>
#include <stdint.h>

#include "../ecs/ecs.h"
#include "colorado/triangle-shader.h"
#include "terf/terf.h"

struct ShaderBinder {
	ShaderBinder ();
	
	ShaderBinder (const ShaderBinder &) = delete;
	ShaderBinder & operator = (const ShaderBinder &) = delete;
	
	void addShader (const ShaderKey, Colorado::TriangleShader *);
	
	const Colorado::TriangleShader * currentShader () const;
	
	void bind (const ShaderKey);
	
private:
	const Colorado::TriangleShader * m_currentShader;
	
	std::map <ShaderKey, std::unique_ptr <const Colorado::TriangleShader> > shaders;
	void bind (const Colorado::TriangleShader *);
	
	const Colorado::TriangleShader * getShader (const ShaderKey) const;
};

Colorado::TriangleShader * newShader (
	const Terf::Archive & terf, 
	const std::string & vertFn, 
	const std::string & fragFn);

#endif // SHADERBINDER_H
