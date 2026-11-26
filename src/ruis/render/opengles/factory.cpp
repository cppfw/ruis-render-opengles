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

#include "factory.hpp"

#include <array>
#include <stdexcept>

#include <utki/config.hpp>
#include <utki/string.hpp>

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

#include "shaders/shader_color.hpp"
#include "shaders/shader_color_pos_lum.hpp"
#include "shaders/shader_color_pos_tex.hpp"
#include "shaders/shader_color_pos_tex_alpha.hpp"
#include "shaders/shader_pos_clr.hpp"
#include "shaders/shader_pos_tex.hpp"

#include "index_buffer.hpp"
#include "texture_2d.hpp"
#include "texture_cube.hpp"
#include "texture_depth.hpp"
#include "util.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"

using namespace std::string_view_literals;

using namespace ruis::render::opengles;

namespace {
capabilities get_capabilities()
{
	auto* exts_str = glGetString(GL_EXTENSIONS);
	assert_opengl_no_error();

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	std::string_view exts_sv(reinterpret_cast<const char*>(exts_str));

	auto exts = utki::split(exts_sv, ' ');

	return capabilities{.oes_element_index_uint = utki::contains(exts, "GL_OES_element_index_uint"sv)};
}
} // namespace

factory::factory(utki::shared_ref<ruis::render::native_window> native_window) :
	native_window(std::move(native_window)),
	caps([&]() {
		// bind the headless GL context and leave it current so that object
		// creation (make_*) operates on the shared GL context
		// (a qualified, non-virtual call: the constructor must not rely on
		// virtual dispatch)
		this->ruis::render::opengles::factory::bind();
		return get_capabilities();
	}())
{}

void factory::bind() const
{
	this->bind(this->native_window.get());
}

utki::shared_ref<ruis::render::factory::shaders> factory::make_shaders() const
{
	auto ret = utki::make_shared<ruis::render::factory::shaders>();
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	ret.get().pos_tex = std::make_unique<shader_pos_tex>(this->get_shared_ref());
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	ret.get().color_pos = std::make_unique<shader_color>(this->get_shared_ref());
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	ret.get().pos_clr = std::make_unique<shader_pos_clr>(this->get_shared_ref());
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	ret.get().color_pos_tex = std::make_unique<shader_color_pos_tex>(this->get_shared_ref());
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	ret.get().color_pos_tex_alpha = std::make_unique<shader_color_pos_tex_alpha>(this->get_shared_ref());
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	ret.get().color_pos_lum = std::make_unique<shader_color_pos_lum>(this->get_shared_ref());
	return ret;
}

utki::shared_ref<ruis::render::texture_2d> factory::make_texture_2d(
	rasterimage::format format, //
	rasterimage::dimensioned::dimensions_type dims,
	texture_2d_parameters params
) const
{
	return this->create_texture_2d_internal(
		format, //
		dims,
		{},
		std::move(params)
	);
}

utki::shared_ref<ruis::render::texture_2d> factory::make_texture_2d(
	const rasterimage::image_variant& imvar, //
	texture_2d_parameters params
) const
{
	auto imvar_copy = imvar;
	return this->make_texture_2d(
		std::move(imvar_copy), //
		std::move(params)
	);
}

utki::shared_ref<ruis::render::texture_2d> factory::make_texture_2d(
	rasterimage::image_variant&& imvar, //
	texture_2d_parameters params
) const
{
	auto iv = std::move(imvar);
	return std::visit(
		[this, &imvar = iv, &params](auto&& im) -> utki::shared_ref<ruis::render::texture_2d> {
			if constexpr (sizeof(im.pixels().front().front()) != 1) {
				throw std::logic_error(
					"factory::make_texture_2d(): "
					"non-8bit images are not supported"
				);
			} else {
				im.span().flip_vertical();
				auto data = im.pixels();
				return this->create_texture_2d_internal(
					imvar.get_format(),
					im.dims(),
					utki::make_span(data.front().data(), data.size_bytes()),
					std::move(params)
				);
			}
		},
		iv.variant
	);
}

utki::shared_ref<ruis::render::texture_2d> factory::create_texture_2d_internal(
	rasterimage::format type, //
	rasterimage::dimensioned::dimensions_type dims,
	utki::span<const uint8_t> data,
	texture_2d_parameters params
) const
{
	return utki::make_shared<texture_2d>(
		this->get_shared_ref(), //
		type,
		dims,
		data,
		params
	);
}

utki::shared_ref<ruis::render::texture_depth> factory::make_texture_depth( //
	rasterimage::dimensioned::dimensions_type dims
) const
{
	return utki::make_shared<texture_depth>(
		this->get_shared_ref(), //
		dims
	);
}

utki::shared_ref<ruis::render::texture_cube> factory::make_texture_cube(
	rasterimage::image_variant&& positive_x, //
	rasterimage::image_variant&& negative_x,
	rasterimage::image_variant&& positive_y,
	rasterimage::image_variant&& negative_y,
	rasterimage::image_variant&& positive_z,
	rasterimage::image_variant&& negative_z
) const
{
	constexpr auto num_cube_sides = 6;
	std::array<rasterimage::image_variant, num_cube_sides> sides = {
		std::move(positive_x),
		std::move(negative_x),

		// negative_y and positive_y are swapped due to texture coordinates y-axis going downwards
		std::move(negative_y),
		std::move(positive_y),

		std::move(positive_z),
		std::move(negative_z)
	};
	std::array<texture_cube::cube_face_image, num_cube_sides> faces;

	auto face = faces.begin();
	for (auto& side : sides) {
		utki::assert(face != faces.end(), SL);
		std::visit(
			[&](auto& im) {
				if constexpr (sizeof(im.pixels().front().front()) != 1) {
					throw std::logic_error(
						"factory::make_texture_cube(): "
						"non-8bit images are not supported"
					);
				} else {
					im.span().flip_vertical();
					auto data = im.pixels();

					face->type = side.get_format();
					face->dims = im.dims();
					face->data = utki::make_span(data.front().data(), data.size_bytes());
				}
			},
			side.variant
		);
		++face;
	}

	return utki::make_shared<texture_cube>(
		this->get_shared_ref(), //
		faces
	);
}

utki::shared_ref<ruis::render::vertex_buffer> factory::make_vertex_buffer( //
	utki::span<const r4::vector4<float>> vertices
) const
{
	return utki::make_shared<vertex_buffer>(
		this->get_shared_ref(), //
		vertices
	);
}

utki::shared_ref<ruis::render::vertex_buffer> factory::make_vertex_buffer( //
	utki::span<const r4::vector3<float>> vertices
) const
{
	return utki::make_shared<vertex_buffer>(
		this->get_shared_ref(), //
		vertices
	);
}

utki::shared_ref<ruis::render::vertex_buffer> factory::make_vertex_buffer( //
	utki::span<const r4::vector2<float>> vertices
) const
{
	return utki::make_shared<vertex_buffer>(
		this->get_shared_ref(), //
		vertices
	);
}

utki::shared_ref<ruis::render::vertex_buffer> factory::make_vertex_buffer( //
	utki::span<const float> vertices
) const
{
	return utki::make_shared<vertex_buffer>(
		this->get_shared_ref(), //
		vertices
	);
}

utki::shared_ref<ruis::render::vertex_array> factory::make_vertex_array(
	std::vector<utki::shared_ref<const ruis::render::vertex_buffer>> buffers, //
	utki::shared_ref<const ruis::render::index_buffer> indices,
	ruis::render::vertex_array::mode rendering_mode
) const
{
	return utki::make_shared<vertex_array>(
		this->get_shared_ref(), //
		std::move(buffers),
		std::move(indices),
		rendering_mode
	);
}

utki::shared_ref<ruis::render::index_buffer> factory::make_index_buffer( //
	utki::span<const uint16_t> indices
) const
{
	return utki::make_shared<index_buffer>(
		this->get_shared_ref(), //
		indices
	);
}

utki::shared_ref<ruis::render::index_buffer> factory::make_index_buffer( //
	utki::span<const uint32_t> indices
) const
{
	if (!this->caps.oes_element_index_uint) {
		throw std::runtime_error("This OpenGL ES implementation does not support 32bit vertex indices");
	}
	return utki::make_shared<index_buffer>(
		this->get_shared_ref(), //
		indices
	);
}
