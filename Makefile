UNAME_S:= $(shell uname -s)

CC=g++

# Folders
SRCDIR:=src Multitask_Env
BUILDDIR:= build
TARGETDIR:= bin

# Targets
EXECUTABLE:= mul
TARGET:= $(TARGETDIR)/$(EXECUTABLE)

# Code Lists
SRCEXT:= cpp
SOURCES:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Folder Lists
INCDIRS:=$(shell find inc/* -name '*.h' -exec dirname {} \; | sort | uniq)
INCLIST:= $(patsubst inc/%,-I inc/%,$(INCDIRS))
BUILDLIST:= $(patsubst inc/%,$(BUILDDIR)/%,$(INCDIRS)) 

# Shared Compiler Flags
CFLAGS:= -std=c++11 
INC:= -I inc $(INCLIST) -I

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@echo " $(CC) $^ -g -o $(TARGET)"; $(CC) $^ -g -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	@echo " $(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<"; $(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<

debug:
	valgrind --leak-check=full --show-leak-kinds=all $(TARGET) task2.cfg 50 1000
clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(OBJDIR) $(TARGET)"; $(RM) -r $(OBJDIR) $(TARGET) 


