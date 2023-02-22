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

#include "render_factory.hpp"

#include "vertex_buffer.hpp"
#include "vertex_array.hpp"

#include "util.hpp"
#include "index_buffer.hpp"

#include "texture_2d.hpp"
#include "shader_pos_tex.hpp"
#include "shader_color.hpp"
#include "shader_pos_clr.hpp"
#include "shader_color_pos_tex.hpp"
#include "shader_color_pos_lum.hpp"
#include "frame_buffer.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace morda::render_opengles;

render_factory::render_factory(){}

render_factory::~render_factory()noexcept{}

utki::shared_ref<morda::texture_2d> render_factory::create_texture_2d(
		morda::texture_2d::type type,
		r4::vector2<unsigned> dims,
		utki::span<const uint8_t> data
	)
{
	//TODO: turn these asserts to real checks with exceptions throwing
	ASSERT(data.size() % morda::texture_2d::bytes_per_pixel(type) == 0)
	ASSERT(data.size() % dims.x() == 0)

	ASSERT(data.size() == 0 || data.size() / morda::texture_2d::bytes_per_pixel(type) / dims.x() == dims.y())
	
	auto ret = utki::make_shared<texture_2d>(dims.to<float>());
	
	//TODO: save previous bind and restore it after?
	ret.get().bind(0);
	
	GLint internalFormat;
	switch(type){
		default:
			ASSERT(false)
		case decltype(type)::grey:
			internalFormat = GL_LUMINANCE;
			break;
		case decltype(type)::grey_alpha:
			internalFormat = GL_LUMINANCE_ALPHA;
			break;
		case decltype(type)::rgb:
			internalFormat = GL_RGB;
			break;
		case decltype(type)::rgba:
			internalFormat = GL_RGBA;
			break;
	}

	// we will be passing pixels to OpenGL which are 1-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	assert_opengl_no_error();

	glTexImage2D(
			GL_TEXTURE_2D,
			0, // 0th level, no mipmaps
			internalFormat, // internal format
			dims.x(),
			dims.y(),
			0, // border, should be 0!
			internalFormat, // format of the texel data
			GL_UNSIGNED_BYTE,
			data.size() == 0 ? nullptr : &*data.begin()
		);
	assert_opengl_no_error();

	// NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//       for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assert_opengl_no_error();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assert_opengl_no_error();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	return ret;
}

utki::shared_ref<morda::vertex_buffer> render_factory::create_vertex_buffer(utki::span<const r4::vector4<float>> vertices){
	return utki::make_shared<vertex_buffer>(vertices);
}

utki::shared_ref<morda::vertex_buffer> render_factory::create_vertex_buffer(utki::span<const r4::vector3<float>> vertices){
	return utki::make_shared<vertex_buffer>(vertices);
}

utki::shared_ref<morda::vertex_buffer> render_factory::create_vertex_buffer(utki::span<const r4::vector2<float>> vertices){
	return utki::make_shared<vertex_buffer>(vertices);
}

utki::shared_ref<morda::vertex_buffer> render_factory::create_vertex_buffer(utki::span<const float> vertices){
	return utki::make_shared<vertex_buffer>(vertices);
}

utki::shared_ref<morda::vertex_array> render_factory::create_vertex_array(
		std::vector<utki::shared_ref<const morda::vertex_buffer>>&& buffers,
		const utki::shared_ref<const morda::index_buffer>& indices,
		morda::vertex_array::mode rendering_mode
	)
{
	return utki::make_shared<vertex_array>(std::move(buffers), indices, rendering_mode);
}

utki::shared_ref<morda::index_buffer> render_factory::create_index_buffer(utki::span<const uint16_t> indices){
	return utki::make_shared<index_buffer>(indices);
}

std::unique_ptr<morda::render_factory::shaders> render_factory::create_shaders(){
	auto ret = std::make_unique<morda::render_factory::shaders>();
	ret->pos_tex = std::make_unique<shader_pos_tex>();
	ret->color_pos = std::make_unique<shader_color>();
	ret->pos_clr = std::make_unique<shader_pos_clr>();
	ret->color_pos_tex = std::make_unique<shader_color_pos_tex>();
	ret->color_pos_lum = std::make_unique<shader_color_pos_lum>();
	return ret;
}

utki::shared_ref<morda::frame_buffer> render_factory::create_framebuffer(
	const utki::shared_ref<morda::texture_2d>& color
)
{
	return utki::make_shared<frame_buffer>(color);
}
