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
GRAPHICAL_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
GRAPHICAL_SRC = src/SDL2.cpp

CXXFLAGS += -Isrc

all: core graphicals

core: $(NAME)

$(NAME): $(OBJ)
	g++ -o $(NAME) $(OBJ) $(CXXFLAGS)

graphicals: $(GRAPHICAL_LIB)

$(GRAPHICAL_LIB): $(GRAPHICAL_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(GRAPHICAL_LIB) $(GRAPHICAL_SRC) $(GRAPHICAL_FLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(GRAPHICAL_LIB)

re: fclean core graphicals