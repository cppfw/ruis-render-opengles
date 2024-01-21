/*
ruis-render-opengles - OpenGL ES GUI renderer

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#pragma once

#include <vector>

#include <r4/matrix.hpp>
#include <ruis/render/vertex_array.hpp>
#include <utki/config.hpp>
#include <utki/debug.hpp>
#include <utki/span.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

#include "util.hpp"

namespace ruis::render_opengles {

struct shader_wrapper {
	GLuint id;
	shader_wrapper(const char* code, GLenum type);

	shader_wrapper(const shader_wrapper&) = delete;
	shader_wrapper& operator=(const shader_wrapper&) = delete;

	shader_wrapper(shader_wrapper&&) = delete;
	shader_wrapper& operator=(shader_wrapper&&) = delete;

	~shader_wrapper()
	{
		glDeleteShader(this->id);
	}
};

struct program_wrapper {
	shader_wrapper vertex_shader;
	shader_wrapper fragment_shader;
	GLuint id;
	program_wrapper(const char* vertex_shader_code, const char* fragment_shader_code);

	program_wrapper(const program_wrapper&) = delete;
	program_wrapper& operator=(const program_wrapper&) = delete;

	program_wrapper(program_wrapper&&) = delete;
	program_wrapper& operator=(program_wrapper&&) = delete;

	~program_wrapper()
	{
		glDeleteProgram(this->id);
	}
};

class shader_base
{
	program_wrapper program;

	const GLint matrix_uniform;

public:
	shader_base(const char* vertex_shader_code, const char* fragment_shader_code);

	shader_base(const shader_base&) = delete;
	shader_base& operator=(const shader_base&) = delete;

	shader_base(shader_base&&) = delete;
	shader_base& operator=(shader_base&&) = delete;

	virtual ~shader_base() = default;

protected:
	GLint get_uniform(const char* name);

	void bind() const
	{
		glUseProgram(program.id);
		assert_opengl_no_error();
	}

	bool is_bound() const noexcept
	{
		GLint prog = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

		ASSERT(prog >= 0)
		return GLuint(prog) == this->program.id;
	}

	void set_uniform_matrix4f(GLint id, const r4::matrix4<float>& m) const
	{
		auto mm = m.tposed();
		glUniformMatrix4fv(
			id,
			1,
			// OpenGL ES2 does not support transposing, see description of
			// 'transpose' parameter of glUniformMatrix4fv():
			// https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glUniform.xml
			GL_FALSE,
			mm.front().data()
		);
		assert_opengl_no_error();
	}

	void set_uniform4f(GLint id, float x, float y, float z, float a) const
	{
		glUniform4f(id, x, y, z, a);
		assert_opengl_no_error();
	}

	void set_matrix(const r4::matrix4<float>& m) const
	{
		this->set_uniform_matrix4f(this->matrix_uniform, m);
		assert_opengl_no_error();
	}

	static const std::array<GLenum, size_t(ruis::vertex_array::mode::enum_size)> mode_map;

	static GLenum mode_to_gl_mode(ruis::vertex_array::mode mode)
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		return mode_map[unsigned(mode)];
	}

	void render(const r4::matrix4<float>& m, const ruis::vertex_array& va) const;
};

} // namespace ruis::render_opengles
