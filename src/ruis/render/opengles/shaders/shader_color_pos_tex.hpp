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

#pragma once

#include <ruis/render/shaders/coloring_texturing_shader.hpp>

#include "../shader_base.hpp"

namespace ruis::render::opengles {

class shader_color_pos_tex :
	public ruis::render::coloring_texturing_shader, //
	public shader_base
{
	GLint texture_uniform;
	GLint color_uniform;

public:
	shader_color_pos_tex(utki::shared_ref<ruis::render::context> render_context);

	shader_color_pos_tex(const shader_color_pos_tex&) = delete;
	shader_color_pos_tex& operator=(const shader_color_pos_tex&) = delete;

	shader_color_pos_tex(shader_color_pos_tex&&) = delete;
	shader_color_pos_tex& operator=(shader_color_pos_tex&&) = delete;

	~shader_color_pos_tex() override = default;

	void render(
		const r4::matrix4<float>& m,
		const ruis::render::vertex_array& va,
		r4::vector4<float> color,
		const ruis::render::texture_2d& tex
	) const override;
};

} // namespace ruis::render::opengles
