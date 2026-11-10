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

#include "frame_buffer.hpp"

#include <vector>

#include <utki/config.hpp>
#include <utki/string.hpp>

#include "texture_2d.hpp"
#include "texture_depth.hpp"
#include "util.hpp"

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace ruis::render::opengles;

frame_buffer::frame_buffer( //
	const utki::shared_ref<ruis::render::context>& rendering_context,
	std::shared_ptr<ruis::render::texture_2d> color,
	std::shared_ptr<ruis::render::texture_depth> depth,
	std::shared_ptr<ruis::render::texture_stencil> stencil
) :
	ruis::render::frame_buffer( //
		rendering_context,
		std::move(color),
		std::move(depth),
		std::move(stencil)
	)
{
	// In OpenGL ES framebuffer objects are not shared between contexts,
	// so make sure the owning context is bound when deleting the framebuffer object.
	this->rendering_context.get().apply([this]() {
		glGenFramebuffers(1, &this->fbo);
		assert_opengl_no_error();

		// the variable is initialized via output argument, so no need to initialize it here
		// NOLINTNEXTLINE(cppcoreguidelines-init-variables)
		GLint old_fb;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fb);

		glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
		assert_opengl_no_error();

		if (this->color) {
			ASSERT(dynamic_cast<texture_2d*>(this->color.get()))
			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
			auto& tex = static_cast<texture_2d&>(*this->color);

			glFramebufferTexture2D(
				GL_FRAMEBUFFER, //
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D,
				tex.tex,
				0
			);
			assert_opengl_no_error();
		}

		if (this->depth) {
			ASSERT(dynamic_cast<texture_depth*>(this->depth.get()))
			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
			auto& tex = static_cast<texture_depth&>(*this->depth);

			glFramebufferTexture2D(
				GL_FRAMEBUFFER, //
				GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D,
				tex.tex,
				0
			);
			assert_opengl_no_error();
		}

		if (this->stencil) {
			throw std::logic_error("frame_buffer(): OpenGL ES stencil texture support is not implemented");
		}

		// Check for completeness
		{
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			assert_opengl_no_error();
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				throw std::runtime_error(
					utki::cat("frame_buffer(): OpenGL ES framebuffer is incomplete: status = ", unsigned(status))
				);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, old_fb);
		assert_opengl_no_error();
	});
}

frame_buffer::~frame_buffer()
{
	// In OpenGL ES framebuffer objects are not shared between contexts,
	// so make sure the owning context is bound when deleting the framebuffer object.
	this->rendering_context.get().apply([this]() {
		glDeleteFramebuffers(1, &this->fbo);
		assert_opengl_no_error();
	});
}

rasterimage::image_variant frame_buffer::get_color_pixels()
{
	std::vector<uint8_t> buf(static_cast<size_t>(this->dims().x()) * this->dims().y() * sizeof(uint32_t));

	// Primary: read the framebuffer. apply() binds this FBO and sets the viewport to the full
	// framebuffer size, restoring both afterwards. This is a probe-only path, so the extra
	// bind/unbind is acceptable. OpenGL ES has no glReadBuffer(); a single color attachment FBO
	// is read from color attachment zero directly.
	bool fbo_read_ok = false;
	this->apply([&]() {
		glReadPixels(
			0, //
			0,
			GLsizei(this->dims().x()),
			GLsizei(this->dims().y()),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			buf.data()
		);
		fbo_read_ok = (glGetError() == GL_NO_ERROR);
	});

	// Unlike desktop OpenGL, OpenGL ES has no glGetTexImage()/glGetTexSubImage(): a texture
	// cannot be read back without a framebuffer, so glReadPixels on the FBO is the only path.
	if (!fbo_read_ok) {
		throw std::runtime_error("opengles::frame_buffer::get_color_pixels(): framebuffer readback failed");
	}

	// glReadPixels returns the rows from the framebuffer's bottom edge upwards, so the captured
	// image is vertically flipped; restore the conventional top-down row order before handing it
	// out (both write_png and the probe transport expect top-down rows).
	auto im = rasterimage::image<uint8_t, sizeof(uint32_t)>::make(this->dims(), buf.data());
	im.span().flip_vertical();
	return {std::move(im)};
}
