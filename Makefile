
# These will be set from the outside
TARGET?=giraffe
SOURCES?=$(shell find src -type f -name '*.cpp' -o -name '*.c')
TOOLCHAIN_NAME?=gcc-11
TOOLCHAIN_CONFIG?=asan
STATIC_LIBCPP?=0
VERBOSE?=0
LTO?=0
UNITY_BUILD?=0
BUILD_TESTS?=0
BUILD_EXAMPLES?=0
BENCHMARK?=0

# -------------------------------------------------------------------------- Configure build options

ifneq ("$(BUILD_TESTS)", "0")
  SOURCES+= $(shell find tests -type f -name '*.cpp' -o -name '*.c')
  CPPFLAGS+= -DCATCH_BUILD -DCATCH_CONFIG_PREFIX_ALL -DCATCH_CONFIG_COLOUR_ANSI -isystemtests
endif

ifneq ("$(BUILD_EXAMPLES)", "0")
  SOURCES+= $(shell find examples -type f -name '*.cpp' -o -name '*.c')
  CPPFLAGS+= -DBUILD_EXAMPLES
endif

ifneq ("$(BENCHMARK)", "0")
  SOURCES+= $(shell find benchmark -type f -name '*.cpp' -o -name '*.c')
  CPPFLAGS+= -DBENCHMARK_BUILD
  LDFLAGS+= -lpthread -L/usr/local/lib -lbenchmark
endif

# Configure includes
INCDIRS:=-Isrc
CXX_CONTRIB_INC:=-isystemcontrib/include
CFLAGS+= $(INCDIRS)
CXXFLAGS+= -DFMT_HEADER_ONLY $(INCDIRS) $(CXX_CONTRIB_INC) -Wno-sign-compare

# -------------------------------------------------------- Check that we're in the correct directory
# Every shell command is executed in the same invocation
MKFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR:=$(patsubst %/,%,$(dir $(MKFILE_PATH)))
ifneq ("$(MKFILE_DIR)", "$(CURDIR)")
  $(error Should run from $(MKFILE_DIR) not $(CURDIR))
endif

# -------------------------------------------------- Include makefile environment and standard rules

include project-config/env.inc.makefile

-include $(DEP_FILES)

# -------------------------------------------------------------------------------------------- Rules
# Standard Rules
.PHONY: clean info test deps test-scan module-deps

all: $(TARGETDIR)/$(TARGET)

test: | all
	$(TARGETDIR)/$(TARGET) src/main.cpp

$(BUILDDIR)/src/scanner/gen-token-types_cpp.o: src/scanner/token-types.h src/scanner/gen-token-types_cpp.sh
	@echo "$(BANNER)gen-token-types $^$(BANEND)"
	src/scanner/gen-token-types_cpp.sh | $(CXX) -x c++ $(CXXFLAGS_F) -c - -o $@
	@$(RECIPETAIL)

$(BUILDDIR)/src/scanner/lexer.o: src/scanner/lexer.l
	@echo "$(BANNER)flex $^$(BANEND)"
	mkdir -p $(dir $@)
	flex -t $< | $(CC) -x c $(CFLAGS_F) \
                           -Wno-sign-compare -Wno-unused-value -Wno-unused-function \
                           -c - -o $@
	@$(RECIPETAIL)

$(TARGETDIR)/$(TARGET): $(OBJECTS) $(BUILDDIR)/src/scanner/gen-token-types_cpp.o $(BUILDDIR)/src/scanner/lexer.o
	@echo "$(BANNER)link $(TARGET)$(BANEND)"
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LDFLAGS_F)
	@$(RECIPETAIL)

clean:
	@echo rm -rf $(BUILDDIR) $(TARGETDIR)
	@rm -rf $(BUILDDIR) $(TARGETDIR)

info:
	@echo "CURDIR:        $(CURDIR)"
	@echo "MKFILE_DIR:    $(MKFILE_DIR)"
	@echo "TARGET:        $(TARGET)"
	@echo "TARGETDIR:     $(TARGETDIR)"
	@echo "COMP_DATABASE: $(COMP_DATABASE)"
	@echo "CONFIG:        $(TOOLCHAIN_CONFIG)"
	@echo "VERBOSE:       $(VERBOSE)"
	@echo "CC:            $(CC)"
	@echo "CXX:           $(CXX)"
	@echo "CFLAGS:        $(CFLAGS_F)"
	@echo "CXXFLAGS:      $(CXXFLAGS_F)"
	@echo "LDFLAGS:       $(LDFLAGS_F)"
	@echo "SOURCES:"
	@echo "$(SOURCES)" | sed 's,^,   ,'
	@echo "OBJECTS:"
	@echo "$(OBJECTS)" | sed 's,^,   ,'
	@echo "DEP_FILES:"
	@echo "$(DEP_FILES)" | sed 's,^,   ,'


