#include <ruis/render/opengles/context.hpp>

#include <iostream>

int main(int argc, const char** argv){
	auto c = utki::make_shared<ruis::render::opengles::context>();

	std::cout << "hello ruis-render-opengles!" << std::endl;

	return 0;
}