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

#include <ruis/render/renderer.hpp>
#include <utki/config.hpp>

#include "factory.hpp"

namespace ruis::render::opengles {

class renderer : public ruis::render::renderer
{
	bool default_framebuffer_initialized = false;

	// NOTE: GLuint is fixed 32bit type, according to
	// OpenGL specs, so use uint32_t.
	uint32_t default_framebuffer = 0;

public:
	renderer(std::unique_ptr<ruis::render::opengles::factory> factory);

	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;

	renderer(renderer&&) = delete;
	renderer& operator=(renderer&&) = delete;

	~renderer() override = default;

	void set_framebuffer_internal(ruis::render::frame_buffer* fb) override;

	void clear_framebuffer_color() override;

	void clear_framebuffer_depth() override;

	void clear_framebuffer_stencil() override;

	r4::vector2<uint32_t> to_window_coords(ruis::vec2 point) const override;

	bool is_scissor_enabled() const noexcept override;

	void enable_scissor(bool enable) override;

	r4::rectangle<uint32_t> get_scissor() const override;

	void set_scissor(r4::rectangle<uint32_t> r) override;

	r4::rectangle<uint32_t> get_viewport() const override;

	void set_viewport(r4::rectangle<uint32_t> r) override;

	void enable_blend(bool enable) override;

	void set_blend_func(
		blend_factor src_color,
		blend_factor dst_color,
		blend_factor src_alpha,
		blend_factor dst_alpha
	) override;

	bool is_depth_enabled() const noexcept override;

	void enable_depth(bool enable) override;
};

} // namespace ruis::render::opengles
