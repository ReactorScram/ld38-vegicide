#ifndef SHADERBINDER_H
#define SHADERBINDER_H

#include <memory>
#include "colorado/triangle-shader.h"
#include "terf/terf.h"

template <class EShader>
struct ShaderBinder {
	ShaderBinder () :
		m_currentShader (nullptr),
		shaders ()
	{
		
	}
	
	ShaderBinder (const ShaderBinder &) = delete;
	ShaderBinder & operator = (const ShaderBinder &) = delete;
	
	void addShader (const EShader code, Colorado::TriangleShader * shader) {
		shaders [code] = std::unique_ptr <Colorado::TriangleShader> (shader);
	}
	
	const Colorado::TriangleShader * currentShader () {
		return m_currentShader;
	}
	
	void bind (const EShader code) {
		bind (getShader (code));
	}
	
private:
	const Colorado::TriangleShader * m_currentShader;
	
	std::map <const EShader, std::unique_ptr <const Colorado::TriangleShader> > shaders;
	void bind (const Colorado::TriangleShader * shader) {
		m_currentShader = shader;
		m_currentShader->bind ();
	}
	
	const Colorado::TriangleShader * getShader (const EShader code) {
		const Colorado::TriangleShader * result = shaders.at (code).get ();
		if (result) {
			return result;
		}
		else {
			//cerr << "Warning: Failed to get shader - " << code << endl;
			return result;
		}
	}
};

Colorado::TriangleShader * newShader (
	const Terf::Archive & terf, 
	const std::string & vertFn, 
	const std::string & fragFn);

#endif // SHADERBINDER_H
