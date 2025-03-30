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

#include "shader_color_pos_tex_alpha.hpp"

#include "../texture_2d.hpp"

using namespace ruis::render::opengles;

shader_color_pos_tex_alpha::shader_color_pos_tex_alpha(utki::shared_ref<ruis::render::context> render_context) :
	ruis::render::coloring_texturing_shader(std::move(render_context)),
	shader_base(
		R"qwertyuiop(
			attribute highp vec4 a0;
			attribute highp vec2 a1;

			uniform highp mat4 matrix;
			
			varying highp vec2 tc0;

			void main(void){
				gl_Position = matrix * a0;
				tc0 = vec2(a1.x, 1.0f - a1.y);
			}
		)qwertyuiop",
		R"qwertyuiop(
			uniform sampler2D texture0;

			uniform highp vec4 uniform_color;

			varying highp vec2 tc0;

			void main(void){
				gl_FragColor = vec4(
					uniform_color.x,
					uniform_color.y,
					uniform_color.z,
					uniform_color.w * texture2D(texture0, tc0).x
				);
			}
		)qwertyuiop"
	),
	texture_uniform(this->get_uniform("texture0")),
	color_uniform(this->get_uniform("uniform_color"))
{}

void shader_color_pos_tex_alpha::render(
	const r4::matrix4<float>& m,
	const ruis::render::vertex_array& va,
	const r4::vector4<float>& color,
	const ruis::render::texture_2d& tex
) const
{
	constexpr auto texture_unit_number = 0;

	ASSERT(dynamic_cast<const texture_2d*>(&tex))
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
	static_cast<const texture_2d&>(tex).bind(texture_unit_number);
	this->bind();

	this->set_uniform_sampler(
		this->texture_uniform, //
		texture_unit_number
	);

	this->set_uniform4f(
		this->color_uniform, //
		color.x(),
		color.y(),
		color.z(),
		color.w()
	);

	this->shader_base::render(m, va);
}
