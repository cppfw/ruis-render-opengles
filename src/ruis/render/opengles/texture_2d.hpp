/*
ruis-render-opengles - OpenGL ES GUI renderer

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

#include <ruis/render/texture_2d.hpp>
#include <utki/config.hpp>

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace ruis::render_opengles {

struct texture_2d : public ruis::texture_2d {
	GLuint tex = 0;

	texture_2d(r4::vector2<float> dims);

	texture_2d(const texture_2d&) = delete;
	texture_2d& operator=(const texture_2d&) = delete;

	texture_2d(texture_2d&&) = delete;
	texture_2d& operator=(texture_2d&&) = delete;

	~texture_2d() override;

	void bind(unsigned unit_num) const;
};

} // namespace ruis::render_opengles
