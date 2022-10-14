/*
morda-render-opengles - OpenGL ES GUI renderer

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

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

const shader_base* shader_base::bound_shader = nullptr;

GLenum shader_base::mode_map[] = {
	GL_TRIANGLES,			// TRIANGLES
	GL_TRIANGLE_FAN,		// TRIANGLE_FAN
	GL_LINE_LOOP,			// LINE_LOOP
	GL_TRIANGLE_STRIP		// TRIANGLE_STRIP
};

namespace{
// return true if not compiled
bool checkForCompileErrors(GLuint shader){
	GLint value = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
	assert_opengl_no_error();
	if(value == 0){ // if not compiled
		GLint log_len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		assert_opengl_no_error();
		if(log_len > 1){ // 1 char is a terminating 0
			std::vector<char> log(log_len);
			GLint len;
			glGetShaderInfoLog(shader, GLsizei(log.size()), &len, &*log.begin());
			assert_opengl_no_error();
			LOG([&](auto&o){o << "===Compile log===\n" << &*log.begin() << std::endl;})
		}else{
			LOG([&](auto&o){o << "Shader compile log is empty" << std::endl;})
		}
		return true;
	}
	return false;
}

// return true if not linked
bool checkForLinkErrors(GLuint program){
	GLint value = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &value);
	assert_opengl_no_error();
	if(value == 0){ // if not linked
		GLint log_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
		assert_opengl_no_error();
		if(log_len > 1){ // 1 is for terminating 0 character.
			std::vector<char> log(log_len);
			GLint len;
			glGetProgramInfoLog(program, GLsizei(log.size()), &len, &*log.begin());
			assert_opengl_no_error();
			LOG([&](auto&o){o << "===Link log===\n" << &*log.begin() << std::endl;})
		}
		return true;
	}
	return false;
}
}

shader_wrapper::shader_wrapper(const char* code, GLenum type){
	this->id = glCreateShader(type);
	assert_opengl_no_error();

	if(this->id == 0){
		throw std::runtime_error("glCreateShader() failed");
	}

	const char* c = code;

	glShaderSource(this->id, 1, &c, 0);
	assert_opengl_no_error();
	glCompileShader(this->id);
	assert_opengl_no_error();
	if(checkForCompileErrors(this->id)) {
		LOG([&](auto&o){o << "Error while compiling:\n" << c << std::endl;})
		glDeleteShader(this->id);
		assert_opengl_no_error();
		throw std::logic_error("Error compiling shader");
	}
}

program_wrapper::program_wrapper(const char* vertex_shader_code, const char* fragment_shader_code) :
		vertex_shader(vertex_shader_code, GL_VERTEX_SHADER),
		fragment_shader(fragment_shader_code, GL_FRAGMENT_SHADER)
{
	this->id = glCreateProgram();
	assert_opengl_no_error();
	glAttachShader(this->id, vertex_shader.id);
	assert_opengl_no_error();
	glAttachShader(this->id, fragment_shader.id);
	assert_opengl_no_error();
	
	GLint max_num_attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_num_attribs);
	assert_opengl_no_error();
	ASSERT(max_num_attribs >= 0)
	
	for(GLuint i = 0; i < GLuint(max_num_attribs); ++i){
		std::stringstream ss;
		ss << "a" << i;
//		TRACE(<< ss.str() << std::endl)
		glBindAttribLocation(this->id, i, ss.str().c_str());
		assert_opengl_no_error();
	}
	
	glLinkProgram(this->id);
	assert_opengl_no_error();
	if(checkForLinkErrors(this->id)){
		LOG([&](auto&o){o << "Error while linking shader program" << vertexShaderCode << std::endl << fragmentShaderCode << std::endl;})
		glDeleteProgram(this->id);
		assert_opengl_no_error();
		throw std::logic_error("Error linking shader program");
	}
}

shader_base::shader_base(const char* vertex_shader_code, const char* fragment_shader_code) :
		program(vertex_shader_code, fragment_shader_code),
		matrix_uniform(this->get_uniform("matrix"))
{}

GLint shader_base::get_uniform(const char* name){
	GLint ret = glGetUniformLocation(this->program.id, name);
	assert_opengl_no_error();
	if(ret < 0){
		throw std::logic_error("No uniform found in the shader program");
	}
	return ret;
}

void shader_base::render(const r4::matrix4<float>& m, const morda::vertex_array& va)const{
	ASSERT(this->is_bound())
	
	this->set_matrix(m);
	
	ASSERT(dynamic_cast<const index_buffer*>(va.indices.get()))
	auto& ivbo = static_cast<const index_buffer&>(*va.indices);

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
