/*
ruis-render-opengl - OpenGL GUI renderer

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

#include <utki/config.hpp>

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace ruis::render::opengles {

struct opengl_texture {
	GLuint tex = 0;

	opengl_texture();

	opengl_texture(const opengl_texture&) = delete;
	opengl_texture& operator=(const opengl_texture&) = delete;

	opengl_texture(opengl_texture&&) = delete;
	opengl_texture& operator=(opengl_texture&&) = delete;

	~opengl_texture();

	void bind(unsigned unit_num) const;
};

} // namespace ruis::render::opengles
