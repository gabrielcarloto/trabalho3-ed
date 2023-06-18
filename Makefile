CXX = g++
CXXFLAGS = -Wall -Wextra

SRCDIR = .
OBJDIR = ./objects

main.exe: $(OBJDIR)/main.o $(OBJDIR)/Menu.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@
