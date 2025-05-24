SRC_DIR = src
INC_DIR = include
object_files = main.o token.o ast.o parser.o mainloop.o

CXX = clang++
LLVM_CONFIG = llvm-config
CPPFLAGS += -I$(INC_DIR)
CPPFLAGS += $(shell $(LLVM_CONFIG) --cxxflags)
LDFLAGS += $(shell $(LLVM_CONFIG) --ldflags --libs)

main: $(object_files)
	$(CXX) $^ -o main $(LDFLAGS) 

$(object_files) : %.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $^ $(CPPFLAGS) -o $@

clean: 
	rm -f $(object_files)
