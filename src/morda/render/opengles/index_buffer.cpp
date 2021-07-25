#include <utki/config.hpp>

#include "index_buffer.hpp"

#include "util.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace morda::render_opengles;

index_buffer::index_buffer(utki::span<const uint16_t> indices) :
		element_type(GL_UNSIGNED_SHORT),
		elements_count(GLsizei(indices.size()))
{	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer);
	assert_opengl_no_error();
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), &*indices.begin(), GL_STATIC_DRAW);
	assert_opengl_no_error();
}
