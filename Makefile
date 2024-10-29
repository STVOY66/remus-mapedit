CC := g++
CV := c++2a #c++ version 20
INC := -I ./src -I ./deps/raylib #include directory flags
LIB := -L ./src -L ./deps/raylib #library directory flags
LFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm #linking flags
WFLAGS := -Wno-narrowing #Remove narrowing warnings
CFLAGS = -g -std=$(CV) $(INC) $(LIB) $(LFLAGS) #consolidates g++ flags

EXENAME := remus-mapedit
MAINSRC := main
SRCDIR := src
OBJDIR := $(SRCDIR)/objs
SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

main: $(OBJ) | $(OBJDIR) #compiles executable using objects
	@echo "Linking executable..."
	@$(CC) -o $(EXENAME) $^ $(CFLAGS)

$(OBJDIR): #creates object directory if one does not exist within the src directory
	@echo "Object directory not found, creating directory..."
	@mkdir $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h | $(OBJDIR) #object compilation
	@echo "Compiling object $@..."
	@$(CC) $(WFLAGS) -c $< $(CFLAGS) -o $@

$(OBJDIR)/$(MAINSRC).o: $(SRCDIR)/$(MAINSRC).cpp | $(OBJDIR) #compiles main source into object
	@echo "Compiling object $@..."
	@$(CC) $(WFLAGS) -c $< $(CFLAGS) -o $@

.phony: clean
clean: #clean function to cleanup aftermath of makefile testing
	@echo "Deleting objects and executables"
	@rm -f $(OBJDIR)/*.o *.exe

.phony: run
run: main
	@./$(EXENAME).exe