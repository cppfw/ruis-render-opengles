include prorab.mk

$(eval $(call prorab-config, ../config))

include $(d)../raspberrypi.mk

this_name := morda-render-opengles

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir,.)

ifeq ($(raspberrypi),true)
    this_cxxflags += -I/opt/vc/include
    this_ldflags += -L/opt/vc/lib
    this_ldlibs += -lbrcmGLESv2
else
    this_ldlibs += `pkg-config --libs glesv2`
endif

this_ldlibs += -lmorda

$(eval $(prorab-build-lib))
