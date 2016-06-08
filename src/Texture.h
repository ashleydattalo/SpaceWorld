#pragma once
#ifndef __Texture__
#define __Texture__

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>

class Texture
{
public:
	Texture();
	virtual ~Texture();
	void setFilename(const std::string &f) { filename = f; }
	void init();
	void setUnit(GLint u) { unit = u; }
	GLint getUnit() const { return unit; }
	void bind(GLint handle);
	void unbind();
	void setWrapModes(GLint wrapS, GLint wrapT); // Must be called after init()
	
	void setFilenamesTexCube(const std::string &negx,
					  const std::string &posx,
					  const std::string &negy,
					  const std::string &posy,
					  const std::string &negz,
					  const std::string &posz);
	void initTexCube();
	void bindTexCube(GLint handle);
	void unbindTexCube();
	
private:
	std::string filename;
	std::string filenames[6];
	int width;
	int height;
	GLuint tid;
	GLint unit;
	
};

#endif
