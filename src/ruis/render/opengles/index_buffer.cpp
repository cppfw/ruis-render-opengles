/*
ruis-render-opengles - OpenGL ES renderer

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "index_buffer.hpp"

#include <utki/config.hpp>

#include "util.hpp"

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace ruis::render::opengles;

index_buffer::index_buffer(
	utki::shared_ref<const ruis::render::context> rendering_context, //
	const void* data,
	size_t size_bytes,
	size_t size,
	GLenum element_type
) :
	ruis::render::index_buffer(std::move(rendering_context)),
	element_type(element_type),
	elements_count(GLsizei(size))
{
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER, //
		this->buffer
	);
	assert_opengl_no_error();

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, //
		GLsizeiptr(size_bytes),
		data,
		GL_STATIC_DRAW
	);
	assert_opengl_no_error();
}

index_buffer::index_buffer(
	utki::shared_ref<const ruis::render::context> rendering_context, //
	utki::span<const uint16_t> indices
) :
	index_buffer(
		std::move(rendering_context), //
		indices.data(),
		indices.size_bytes(),
		indices.size(),
		GL_UNSIGNED_SHORT
	)
{}

index_buffer::index_buffer(
	utki::shared_ref<const ruis::render::context> rendering_context, //
	utki::span<const uint32_t> indices
) :
	index_buffer(
		std::move(rendering_context), //
		indices.data(),
		indices.size_bytes(),
		indices.size(),
		GL_UNSIGNED_INT
	)
{}
