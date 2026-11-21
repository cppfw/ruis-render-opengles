// this file is to test basic usage of the library,
// mainly test that linking to the library works, so just reference any
// non-inline function of the library to test it.

#include <ruis/render/opengles/context.hpp>
#include <ruis/render/opengles/factory.hpp>

#include <iostream>

int main(int argc, const char** argv){
    // reference a non-inline function of the library to test that linking works.
    // do not call it (would require a real GL ES context).
    std::function<void()> f = [](){
        auto nw = utki::make_shared<ruis::render::native_window>();
        auto fac = utki::make_shared<ruis::render::opengles::factory>(nw);
        ruis::render::opengles::context c(nw, fac);
        (void)c;
    };

    if(!f){
        return 1;
    }

    std::cout << "Hello ruis-render-opengles!" << std::endl;
    return 0;
}
