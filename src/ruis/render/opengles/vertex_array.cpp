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

#include "vertex_array.hpp"

#include "index_buffer.hpp"
#include "util.hpp"
#include "vertex_buffer.hpp"

using namespace ruis::render::opengles;

vertex_array::vertex_array(
	utki::shared_ref<const ruis::render::context> rendering_context, //
	buffers_type buffers,
	utki::shared_ref<const ruis::render::index_buffer> indices,
	mode rendering_mode
) :
	ruis::render::vertex_array(
		std::move(rendering_context), //
		std::move(buffers),
		std::move(indices),
		rendering_mode
	)
{}
