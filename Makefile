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
SDL_LIB = lib/arcade_sdl2.so
SDL_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
SDL_SRC = src/SDL2.cpp

NCURSES_LIB = lib/arcade_ncurses.so
NCURSES_SRC = src/Ncurses.cpp

SNAKE_LIB = lib/arcade_snake.so
SNAKE_SRC = src/Snake.cpp
PACMAN_LIB = lib/arcade_pacman.so
PACMAN_SRC = src/Pacman.cpp

CXXFLAGS += -Werror -Wall -Wextra -Isrc

all: core graphicals games

core: $(NAME)

$(NAME): $(OBJ)
	g++ -o $(NAME) $(OBJ) $(CXXFLAGS)

graphicals: $(SDL_LIB) $(NCURSES_LIB)

$(NCURSES_LIB): $(NCURSES_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(NCURSES_LIB) $(NCURSES_SRC) -lncurses

$(SDL_LIB): $(SDL_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(SDL_LIB) $(SDL_SRC) $(SDL_FLAGS)

games: $(SNAKE_LIB) $(PACMAN_LIB)

$(SNAKE_LIB): $(SNAKE_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(SNAKE_LIB) $(SNAKE_SRC)

$(PACMAN_LIB): $(PACMAN_SRC)
	mkdir -p lib
	g++ -shared -fPIC $(CXXFLAGS) -o $(PACMAN_LIB) $(PACMAN_SRC)
clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(SDL_LIB) $(NCURSES_LIB) $(SNAKE_LIB) $(PACMAN_LIB)

re: fclean core graphicals games