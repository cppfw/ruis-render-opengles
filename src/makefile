include prorab.mk
include prorab-license.mk
include prorab-linux.mk
include prorab-clang-format.mk
include prorab-install-dbgsrc.mk

$(eval $(call prorab-config, ../config))

this_name := ruis-render-opengles

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir,.)

this_ldlibs += `pkg-config --libs glesv2`

this_ldlibs += -lruis

$(eval $(prorab-build-lib))

this_license_file := ../LICENSE
this_src_dir := ruis
$(eval $(prorab-license))

$(eval $(prorab-clang-format))

$(eval $(prorab-install-dbgsrc))
