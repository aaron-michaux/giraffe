
# These will be set from the outside
TARGET?=sail
SOURCES?=$(shell find src -type f -name '*.cpp')
#SOURCES:=main.cpp
TOOLCHAIN_NAME?=gcc-11
TOOLCHAIN_CONFIG?=asan
STATIC_LIBCPP?=0
VERBOSE?=0
LTO?=0

# Configure includes
INCDIRS:=-Isrc
CXX_CONTRIB_INC:=-isystemcontrib/include
CFLAGS:=$(CFLAGS) $(INCDIRS)
CXXFLAGS:=$(CXXFLAGS) -DFMT_HEADER_ONLY $(INCDIRS) $(CXX_CONTRIB_INC)

SED:=gsed

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
.PHONY: clean info deps test-scan module-deps

#include /tmp/build-amichaux/gcc-11.2.0-asan/sail/src/main.o.d
#include test1.o.d

all: $(TARGETDIR)/$(TARGET)

comp-database: | $(COMP_DATABASE)

$(COMP_DATABASE): $(COMPDBS)
	@echo '$(BANNER)c++-system-header $<$(BANEND)'
	mkdir -p "$(dir $@)"
	echo "[" > $@
	cat $(COMPDBS) >> $@
	$(SED) -i '$$d' $@
	echo "]" >> $@

$(BUILDDIR)/src/scanner/gen-token-types_c.o: src/scanner/token-types.h src/scanner/gen-token-types_c.sh
	@echo "$(BANNER)gen-token-types $^$(BANEND)"
	src/scanner/gen-token-types_c.sh | $(CC) -x c $(CFLAGS_F) -c - -o $@
	@$(RECIPETAIL)

$(BUILDDIR)/src/scanner/lexer.o: src/scanner/lexer.l
	@echo "$(BANNER)flex $^$(BANEND)"
	flex -t $< | $(CC) -x c $(CFLAGS_F) -Wno-unused-value -Wno-unused-function -c - -o $@
	@$(RECIPETAIL)

$(BUILDDIR)/%.o: %.cpp
	@echo "$(BANNER)c++ $<$(BANEND)"
	mkdir -p $(dir $@)
	$(CXX) -x c++ $(CXXFLAGS_F) -MMD -MF $@.d -c $< -o $@
	@$(RECIPETAIL)

$(BUILDDIR)/%.comp-db.json: %.cpp
	@echo "$(BANNER)comp-db $<$(BANEND)"
	mkdir -p $(dir $@)
	printf "{ \"directory\": \"%s\",\n" "$$(echo "$(CURDIR)" | sed 's,\\,\\\\,g' | sed 's,",\\",g')" > $@
	printf "  \"file\":      \"%s\",\n" "$$(echo "$<" | sed 's,\\,\\\\,g' | sed 's,",\\",g')" >> $@
	printf "  \"command\":   \"%s\",\n" "$$(echo "$(CXX) -x c++ $(CXXFLAGS_F) -c $< -o $@" | sed 's,\\,\\\\,g' | sed 's,",\\",g')" >> $@
	printf "  \"output\":    \"%s\" }\n" "$$(echo "$@" | sed 's,\\,\\\\,g' | sed 's,",\\",g')" >> $@
	printf ",\n" >> $@
	@$(RECIPETAIL)

$(TARGETDIR)/$(TARGET): $(OBJECTS) $(BUILDDIR)/src/scanner/gen-token-types_c.o $(BUILDDIR)/src/scanner/lexer.o
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


