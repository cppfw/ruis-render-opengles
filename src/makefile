include prorab.mk
include prorab-license.mk
include prorab-linux.mk
include prorab-clang-format.mk
include prorab-install-dbgsrc.mk

$(eval $(call prorab-config, ../config))

this_src_dir := ruis

this_name := ruis-render-opengles$(this_dbg)

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir,$(this_src_dir))

this_ldlibs += `pkg-config --libs glesv2`

this_ldlibs += -lruis$(this_dbg)

$(eval $(prorab-build-lib))

this_license_file := ../LICENSE
$(eval $(prorab-license))

$(eval $(prorab-clang-format))

$(eval $(prorab-install-dbgsrc))
