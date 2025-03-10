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

#include <ruis/render/factory.hpp>

#include "capabilities.hpp"
#include "context.hpp"

namespace ruis::render::opengles {

class factory : public ruis::render::factory
{
public:
	factory(utki::shared_ref<ruis::render::context> render_context);

	factory(const factory&) = delete;
	factory& operator=(const factory&) = delete;

	factory(factory&&) = delete;
	factory& operator=(factory&&) = delete;

	~factory() override = default;
};

} // namespace ruis::render::opengles
