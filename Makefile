NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

SRCDIR = src
INCDIR = include
LIBDIR = lib

SOURCES = $(SRCDIR)/main.cpp \
			$(SRCDIR)/Utils.cpp \
			$(SRCDIR)/Command.cpp \
			$(SRCDIR)/Channel.cpp \
			$(SRCDIR)/Client.cpp \
			$(SRCDIR)/Server.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# ekrause test zone, delete before validating the project
ekrause:
	$(CXX) $(CXXFLAGS) $(SRCDIR)/Client.cpp $(SRCDIR)/Channel.cpp $(SRCDIR)/main.cpp -o ekrause

eclean: fclean
	rm -f ekrause
# ///////////////////////////////////////////////////// #

.PHONY: all clean fclean re