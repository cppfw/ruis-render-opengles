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

#include <ruis/render/frame_buffer.hpp>
#include <utki/config.hpp>

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace ruis::render::opengles {

class frame_buffer : public ruis::render::frame_buffer
{
public:
	GLuint fbo = 0;

	frame_buffer( //
		utki::shared_ref<ruis::render::renderer> renderer,
		std::shared_ptr<ruis::render::texture_2d> color,
		std::shared_ptr<ruis::render::texture_depth> depth,
		std::shared_ptr<ruis::render::texture_stencil> stencil
	);

	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;

	frame_buffer(frame_buffer&&) = delete;
	frame_buffer& operator=(frame_buffer&&) = delete;

	~frame_buffer() override;

private:
};

} // namespace ruis::render::opengles
