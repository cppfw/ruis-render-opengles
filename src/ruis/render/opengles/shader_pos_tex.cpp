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

#include "shader_pos_tex.hpp"

#include "index_buffer.hpp"
#include "texture_2d.hpp"
#include "util.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"

using namespace ruis::render_opengles;

shader_pos_tex::shader_pos_tex() :
	shader_base(
		R"qwertyuiop(
						attribute highp vec4 a0; //position

						attribute highp vec2 a1; //texture coordinates

						uniform highp mat4 matrix;

						varying highp vec2 tc0;

						void main(void){
							gl_Position = matrix * a0;
							tc0 = a1;
						}
					)qwertyuiop",
		R"qwertyuiop(
						uniform sampler2D texture0;
		
						varying highp vec2 tc0;
		
						void main(void){
							gl_FragColor = texture2D(texture0, tc0);
						}
					)qwertyuiop"
	),
	texture_uniform(this->get_uniform("texture0"))
{}

void shader_pos_tex::render(const r4::matrix4<float>& m, const ruis::vertex_array& va, const ruis::texture_2d& tex)
	const
{
	ASSERT(dynamic_cast<const texture_2d*>(&tex))
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
	static_cast<const texture_2d&>(tex).bind(0);
	this->bind();

	this->shader_base::render(m, va);
}