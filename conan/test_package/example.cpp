#include <ruis/render/opengles/renderer.hpp>

int main(int argc, const char** argv){
	auto r = utki::make_shared<ruis::render::opengles::renderer>();

	std::cout << "hello ruis-render-opengles!" << std::endl;

	return 0;
}