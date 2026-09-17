##
## EPITECH PROJECT, 2025
## G-OOP-400-LIL-4-1-arcade-9
## File description:
## Makefile
##

SRC = 	main.cpp \
		src/Core.cpp \

OBJ = $(SRC:.cpp=.o)

NAME = arcade
GRAPHICAL_LIB = lib/arcade_sdl2.so
GRAPHICAL_SRC = src/SDL2.cpp

CXXFLAGS += -Isrc

core: $(NAME)

$(NAME): $(OBJ)
	g++ -o $(NAME) $(OBJ) $(CXXFLAGS)

graphicals: $(GRAPHICAL_LIB)

$(GRAPHICAL_LIB): $(GRAPHICAL_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(GRAPHICAL_LIB) $(GRAPHICAL_SRC)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean core