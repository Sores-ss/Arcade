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

GAME_LIB = lib/arcade_snake.so
GAME_SRC = src/Snake.cpp

CXXFLAGS += -Isrc

all: core graphicals games

core: $(NAME)

$(NAME): $(OBJ)
	g++ -o $(NAME) $(OBJ) $(CXXFLAGS)

graphicals: $(GRAPHICAL_LIB)

$(GRAPHICAL_LIB): $(GRAPHICAL_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(GRAPHICAL_LIB) $(GRAPHICAL_SRC) $(GRAPHICAL_FLAGS)

games: $(GAME_LIB)

$(GAME_LIB): $(GAME_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(GAME_LIB) $(GAME_SRC)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(GRAPHICAL_LIB) $(GAME_LIB)

re: fclean core graphicals games