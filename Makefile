CXX := g++
CXXFLAGS := -std=c++20 -O3 -Wall -Wextra -Werror -pedantic -pthread
INCLUDE := -Iinclude

BUILDDIR := build
SRCS := $(wildcard *.cpp)

TARGET := program

OBJECTS := $(SRCS:%.cpp=$(BUILDDIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

all: $(BUILDDIR) $(TARGET)

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

$(BUILDDIR):
	@mkdir -p $@

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)
	@rm -f $(TARGET)