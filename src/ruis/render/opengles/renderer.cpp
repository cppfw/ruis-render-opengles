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

#include "renderer.hpp"

#include <utki/config.hpp>

#include "frame_buffer.hpp"
#include "util.hpp"

#if CFG_OS_NAME == CFG_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace ruis::render::opengles;

renderer::renderer(std::unique_ptr<ruis::render::opengles::factory> factory) :
	ruis::render::renderer(
		std::move(factory),
		{// .max_texture_size =
		 //  []() {
		 // 	 // the variable is initialized via output argument, so no need to initialize it here
		 // 	 // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
		 // 	 GLint val;
		 // 	 glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
		 // 	 assert_opengl_no_error();
		 // 	 ASSERT(val > 0, [&](auto& o) {
		 // 		 o << "val = " << val;
		 // 	 })
		 // 	 return unsigned(val);
		 //  }(),
		 .initial_matrix = ruis::matrix4()
							   // OpenGL identity matrix:
							   //   viewport edges: left = -1, right = 1, top = 1, bottom = -1
							   //   z-axis towards viewer
							   .set_identity()
							   // x-axis right, y-axis down, z-axis away
							   .scale(1, -1, -1)
							   // viewport edges: left = 0, top = 0
							   .translate(-1, -1)
							   // viewport edges: right = 1, bottom = 1
							   .scale(2, 2)
		}
	)
{
	glEnable(GL_CULL_FACE);
}

void renderer::set_framebuffer_internal(ruis::render::frame_buffer* fb)
{
	if (!this->default_framebuffer_initialized) {
		// On some platforms the default framebuffer is not 0, so because of this
		// check if default framebuffer value is saved or not every time some
		// framebuffer is going to be bound and save the value if needed.

		// the variable is initialized via output argument, so no need to initialize it here
		// NOLINTNEXTLINE(cppcoreguidelines-init-variables)
		GLint old_fb;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fb);
		LOG([&](auto& o) {
			o << "old_fb = " << old_fb << std::endl;
		})
		this->default_framebuffer = decltype(this->default_framebuffer)(old_fb);
		this->default_framebuffer_initialized = true;
	}

	if (!fb) {
		glBindFramebuffer(GL_FRAMEBUFFER, GLuint(this->default_framebuffer));
		assert_opengl_no_error();
		return;
	}

	ASSERT(dynamic_cast<frame_buffer*>(fb))
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
	auto& ogl2fb = static_cast<frame_buffer&>(*fb);

	glBindFramebuffer(GL_FRAMEBUFFER, ogl2fb.fbo);
	assert_opengl_no_error();
}

void renderer::clear_framebuffer_color()
{
	// Default clear color is RGBA = (0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	assert_opengl_no_error();
}

void renderer::clear_framebuffer_depth()
{
	// Default clear depth value is 1, see glClearDepthf()
	glClear(GL_DEPTH_BUFFER_BIT);
	assert_opengl_no_error();
}

void renderer::clear_framebuffer_stencil()
{
	// Default clear stencil value is 0, see glClearStencil()
	glClear(GL_STENCIL_BUFFER_BIT);
	assert_opengl_no_error();
}

r4::vector2<uint32_t> renderer::to_window_coords(ruis::vec2 point) const
{
	auto vp = this->get_viewport();

	point += ruis::vec2(1, 1);
	point = max(point, {0, 0}); // clamp to >= 0
	point /= 2;
	point.comp_multiply(vp.d.to<real>());
	point = round(point);
	return point.to<uint32_t>() + vp.p;
}

bool renderer::is_scissor_enabled() const noexcept
{
	return glIsEnabled(GL_SCISSOR_TEST) ? true : false; // "? true : false" is to avoid warning under MSVC
}

void renderer::enable_scissor(bool enable)
{
	if (enable) {
		glEnable(GL_SCISSOR_TEST);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
}

r4::rectangle<uint32_t> renderer::get_scissor() const
{
	std::array<GLint, 4> osb{};
	glGetIntegerv(GL_SCISSOR_BOX, osb.data());

#ifdef DEBUG
	for (auto n : osb) {
		ASSERT(n >= 0)
	}
#endif

	return {
		uint32_t(osb[0]), //
		uint32_t(osb[1]),
		uint32_t(osb[2]),
		uint32_t(osb[3])
	};
}

void renderer::set_scissor(r4::rectangle<uint32_t> r)
{
	glScissor(GLint(r.p.x()), GLint(r.p.y()), GLint(r.d.x()), GLint(r.d.y()));
	assert_opengl_no_error();
}

r4::rectangle<uint32_t> renderer::get_viewport() const
{
	std::array<GLint, 4> vp{};

	glGetIntegerv(GL_VIEWPORT, vp.data());

#ifdef DEBUG
	for (auto n : vp) {
		ASSERT(n >= 0)
	}
#endif

	return {
		uint32_t(vp[0]), //
		uint32_t(vp[1]),
		uint32_t(vp[2]),
		uint32_t(vp[3])
	};
}

void renderer::set_viewport(r4::rectangle<uint32_t> r)
{
	glViewport(GLint(r.p.x()), GLint(r.p.y()), GLint(r.d.x()), GLint(r.d.y()));
	assert_opengl_no_error();
}

void renderer::enable_blend(bool enable)
{
	if (enable) {
		glEnable(GL_BLEND);
	} else {
		glDisable(GL_BLEND);
	}
}

namespace {

const std::array<GLenum, size_t(ruis::render::renderer::blend_factor::enum_size)> blend_func = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_SRC_ALPHA_SATURATE
};

} // namespace

void renderer::set_blend_func(
	blend_factor src_color,
	blend_factor dst_color,
	blend_factor src_alpha,
	blend_factor dst_alpha
)
{
	glBlendFuncSeparate(
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		blend_func[unsigned(src_color)],
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		blend_func[unsigned(dst_color)],
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		blend_func[unsigned(src_alpha)],
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		blend_func[unsigned(dst_alpha)]
	);
}

bool renderer::is_depth_enabled() const noexcept
{
	return glIsEnabled(GL_DEPTH_TEST) ? true : false; // "? true : false" is to avoid warning under MSVC
}

void renderer::enable_depth(bool enable)
{
	if (enable) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}
