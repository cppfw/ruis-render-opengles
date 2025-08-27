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

#include "shader_color_pos_lum.hpp"

using namespace ruis::render::opengles;

shader_color_pos_lum::shader_color_pos_lum(utki::shared_ref<const ruis::render::context> rendering_context) :
	ruis::render::coloring_shader(std::move(rendering_context)),
	shader_base(
		R"qwertyuiop(
			attribute highp vec4 a0;
			attribute highp float a1;

			uniform highp mat4 matrix;

			varying highp float lum;

			void main(void){
				gl_Position = matrix * a0;
				lum = a1;
			}
		)qwertyuiop",
		R"qwertyuiop(
			uniform lowp vec4 uniform_color;

			varying highp float lum;

			void main(void){
				gl_FragColor = vec4(uniform_color.x, uniform_color.y, uniform_color.z, uniform_color.w * lum);
			}
		)qwertyuiop"
	),
	color_uniform(this->get_uniform("uniform_color"))
{}

void shader_color_pos_lum::render(
	const r4::matrix4<float>& m,
	const ruis::render::vertex_array& va,
	const r4::vector4<float>& color
) const
{
	this->bind();

	this->set_uniform4f(
		this->color_uniform, //
		color.x(),
		color.y(),
		color.z(),
		color.w()
	);

	this->shader_base::render(m, va);
}
