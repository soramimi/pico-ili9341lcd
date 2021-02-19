
TEMPLATE = app
TARGET = ili9341
INCLUDEPATH += $(HOME)/pico/pico-sdk/src/common/pico_base/include
INCLUDEPATH += $(HOME)/pico/pico-sdk/src/common/pico_binary_info/include
INCLUDEPATH += $(HOME)/pico/pico-sdk/src/common/pico_stdlib/include
INCLUDEPATH += $(HOME)/pico/pico-sdk/src/rp2_common/hardware_base/include
INCLUDEPATH += $(HOME)/pico/pico-sdk/src/rp2_common/hardware_spi/include

HEADERS += build/generated/pico_base/pico/config_autogen.h \
		   build/generated/pico_base/pico/version.h

SOURCES += \
           build/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.c \
           build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.cpp \
           build/elf2uf2/CMakeFiles/3.16.3/CompilerIdC/CMakeCCompilerId.c \
           build/elf2uf2/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.cpp \
           main.cpp
