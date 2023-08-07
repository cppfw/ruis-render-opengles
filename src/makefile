include prorab.mk
include prorab-license.mk
include prorab-linux.mk
include prorab-clang-format.mk

$(eval $(call prorab-config, ../config))

this_name := morda-render-opengles

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir,.)

ifeq ($(prorab_linux),raspbian)
    this_cxxflags += -isystem /opt/vc/include
    this_ldflags += -L/opt/vc/lib
    this_ldlibs += -lbrcmGLESv2
else
    this_ldlibs += `pkg-config --libs glesv2`
endif

this_ldlibs += -lmorda

$(eval $(prorab-build-lib))

this_license_file := ../LICENSE
this_src_dir := .
$(eval $(prorab-license))

$(eval $(prorab-clang-format))
