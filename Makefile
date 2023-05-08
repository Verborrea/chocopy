CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
SRCDIR = src
INCDIR = include
BUILDDIR = build
TARGET = choco.exe

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
DEPS := $(wildcard $(INCDIR)/*.hpp)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR) $(TARGET)