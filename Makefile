.DEFAULT_GOAL := all
SHELL := cmd.exe
.SHELLFLAGS := /C
.DELETE_ON_ERROR:

TOOLCHAIN ?= $(firstword $(foreach dir,C:/msys64/ucrt64/bin C:/msys64/mingw64/bin,$(if $(wildcard $(dir)/g++.exe),$(dir))))
ifneq ($(strip $(TOOLCHAIN)),)
export PATH := $(TOOLCHAIN);$(PATH)
endif
ifeq ($(origin CC),default)
CC := $(if $(TOOLCHAIN),"$(TOOLCHAIN)/gcc.exe",gcc)
endif
ifeq ($(origin CXX),default)
CXX := $(if $(TOOLCHAIN),"$(TOOLCHAIN)/g++.exe",g++)
endif

CPPFLAGS := -Iinclude -Ithird_party/glfw/include -Ithird_party/glad/include
CFLAGS := -O2 -std=c99 -D_GLFW_WIN32
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS := -static
LDLIBS := -lgdi32 -luser32 -lshell32

GLFW_UNITS := context init input monitor platform vulkan window \
    egl_context osmesa_context null_init null_monitor null_window null_joystick \
    win32_module win32_time win32_thread win32_init win32_joystick \
    win32_monitor win32_window wgl_context
APP_SOURCES := $(wildcard src/app/*.cpp src/graphics/*.cpp src/scene/*.cpp src/input/*.cpp src/ui/*.cpp)
APP_OBJECTS := $(patsubst src/%.cpp,build/make-obj/%.o,$(APP_SOURCES))
GLFW_OBJECTS := $(addprefix build/make-obj/glfw/,$(addsuffix .o,$(GLFW_UNITS)))
OBJECTS := $(APP_OBJECTS) $(GLFW_OBJECTS)
SHADERS := $(patsubst shaders/%,build/shaders/%,$(wildcard shaders/*.vert shaders/*.frag))

.PHONY: all run clean smoke-test
all: build/server_room.exe $(SHADERS)

build/server_room.exe: $(OBJECTS) Makefile
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o "$@"

# Each subdirectory is created once, including during parallel builds.
build/make-obj/app build/make-obj/graphics build/make-obj/scene build/make-obj/input build/make-obj/ui build/make-obj/glfw build/shaders:
	@if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"

build/make-obj/%.o: src/%.cpp Makefile | build/make-obj/app build/make-obj/graphics build/make-obj/scene build/make-obj/input build/make-obj/ui
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c "$<" -o "$@"

build/make-obj/glfw/%.o: third_party/glfw/src/%.c Makefile | build/make-obj/glfw
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c "$<" -o "$@"

build/shaders/%: shaders/% | build/shaders
	copy /Y "$(subst /,\,$<)" "$(subst /,\,$@)" >nul

run: all
	.\build\server_room.exe

smoke-test: all
	.\build\server_room.exe --smoke-test

clean:
	@if exist build rmdir /S /Q build

-include $(OBJECTS:.o=.d)
