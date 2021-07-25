#include <utki/config.hpp>
#include <utki/debug.hpp>

#include <vector>

#include "shader_base.hpp"
#include "util.hpp"
#include "vertex_array.hpp"
#include "index_buffer.hpp"
#include "vertex_buffer.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace morda::render_opengles;

const shader_base* shader_base::boundShader = nullptr;

GLenum shader_base::mode_map[] = {
	GL_TRIANGLES,			// TRIANGLES
	GL_TRIANGLE_FAN,		// TRIANGLE_FAN
	GL_LINE_LOOP,			// LINE_LOOP
	GL_TRIANGLE_STRIP		// TRIANGLE_STRIP
};

namespace{
//return true if not compiled
bool checkForCompileErrors(GLuint shader) {
	GLint value = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
	assert_opengl_no_error();
	if (value == 0) { //if not compiled
		GLint logLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		assert_opengl_no_error();
		if (logLen > 1) {//1 char is a terminating 0
			std::vector<char> log(logLen);
			GLint len;
			glGetShaderInfoLog(shader, GLsizei(log.size()), &len, &*log.begin());
			assert_opengl_no_error();
			TRACE( << "===Compile log===\n" << &*log.begin() << std::endl)
		} else {
			TRACE( << "Shader compile log is empty" << std::endl)
		}
		return true;
	}
	return false;
}

//return true if not linked
bool checkForLinkErrors(GLuint program){
	GLint value = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &value);
	assert_opengl_no_error();
	if(value == 0){ //if not linked
		GLint logLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		assert_opengl_no_error();
		if(logLen > 1){ //1 is for terminating 0 character.
			std::vector<char> log(logLen);
			GLint len;
			glGetProgramInfoLog(program, GLsizei(log.size()), &len, &*log.begin());
			assert_opengl_no_error();
			TRACE(<< "===Link log===\n" << &*log.begin() << std::endl)
		}
		return true;
	}
	return false;
}
}

shader_wrapper::shader_wrapper(const char* code, GLenum type) {
	this->s = glCreateShader(type);
	assert_opengl_no_error();

	if (this->s == 0) {
		throw std::runtime_error("glCreateShader() failed");
	}

	const char* c = code;

	glShaderSource(this->s, 1, &c, 0);
	assert_opengl_no_error();
	glCompileShader(this->s);
	assert_opengl_no_error();
	if(checkForCompileErrors(this->s)) {
		TRACE( << "Error while compiling:\n" << c << std::endl)
		glDeleteShader(this->s);
		assert_opengl_no_error();
		throw std::logic_error("Error compiling shader");
	}
}

program_wrapper::program_wrapper(const char* vertexShaderCode, const char* fragmentShaderCode) :
		vertexShader(vertexShaderCode, GL_VERTEX_SHADER),
		fragmentShader(fragmentShaderCode, GL_FRAGMENT_SHADER)
{
	this->p = glCreateProgram();
	assert_opengl_no_error();
	glAttachShader(this->p, vertexShader.s);
	assert_opengl_no_error();
	glAttachShader(this->p, fragmentShader.s);
	assert_opengl_no_error();
	
	GLint maxAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
	assert_opengl_no_error();
	ASSERT(maxAttribs >= 0)
	
	for(GLuint i = 0; i < GLuint(maxAttribs); ++i){
		std::stringstream ss;
		ss << "a" << i;
//		TRACE(<< ss.str() << std::endl)
		glBindAttribLocation(this->p, i, ss.str().c_str());
		assert_opengl_no_error();
	}
	
	glLinkProgram(this->p);
	assert_opengl_no_error();
	if (checkForLinkErrors(this->p)) {
		TRACE( << "Error while linking shader program" << vertexShaderCode << std::endl << fragmentShaderCode << std::endl)
		glDeleteProgram(this->p);
		assert_opengl_no_error();
		throw std::logic_error("Error linking shader program");
	}
}

shader_base::shader_base(const char* vertexShaderCode, const char* fragmentShaderCode) :
		program(vertexShaderCode, fragmentShaderCode),
		matrixUniform(this->get_uniform("matrix"))
{
}

GLint shader_base::get_uniform(const char* n) {
	GLint ret = glGetUniformLocation(this->program.p, n);
	assert_opengl_no_error();
	if(ret < 0){
		throw std::logic_error("No uniform found in the shader program");
	}
	return ret;
}

void shader_base::render(const r4::matrix4<float>& m, const morda::vertex_array& va)const{
	ASSERT(this->is_bound())
	
	ASSERT(dynamic_cast<const index_buffer*>(va.indices.get()))
	auto& ivbo = static_cast<const index_buffer&>(*va.indices);
	
	this->set_matrix(m);
	
	for(unsigned i = 0; i != va.buffers.size(); ++i){
		ASSERT(dynamic_cast<vertex_buffer*>(va.buffers[i].get()))
		auto& vbo = static_cast<vertex_buffer&>(*va.buffers[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
		assert_opengl_no_error();
		
//		TRACE(<< "vbo.numComponents = " << vbo.num_components << " vbo.type = " << vbo.type << std::endl)
		
		glVertexAttribPointer(i, vbo.num_components, vbo.type, GL_FALSE, 0, nullptr);
		assert_opengl_no_error();
		
		glEnableVertexAttribArray(i);
		assert_opengl_no_error();
	}
	
	{
		ASSERT(dynamic_cast<index_buffer*>(va.indices.get()))
		auto& ivbo = static_cast<index_buffer&>(*va.indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo.buffer);
		assert_opengl_no_error();
	}

//	TRACE(<< "ivbo.elementsCount = " << ivbo.elementsCount << " ivbo.elementType = " << ivbo.elementType << std::endl)
	
	glDrawElements(mode_to_gl_mode(va.rendering_mode), ivbo.elements_count, ivbo.element_type, nullptr);
	assert_opengl_no_error();
}

