# Arcade

## Overview

Arcade is an extensible game launcher framework that supports dynamic loading of graphical libraries and game modules. The architecture allows multiple renderers (SDL2, GTK+, Ncurses) and games (Snake, Pacman) to be swapped at runtime without recompilation.

The core system:
- Discovers and loads `.so` libraries from `./lib`
- Manages a shared player profile and score tracking
- Provides a menu interface for game selection and renderer switching
- Ensures compatibility between different rendering backends and game implementations

## Usage

```bash
./arcade ./lib/arcade_sdl2.so
```

Provide a graphical library path to launch. The system will auto-discover all compatible game libraries from `./lib`.

## Controls

| Key | Action |
|-----|--------|
| **Arrow Keys** | Navigate menus / Control player |
| **Enter** | Select menu option / Confirm action |
| **Escape** | Exit current menu |
| **Tab** | Switch to next graphical library |
| **Backspace** | Delete character in text input |
| **Delete** | Quit the program |
| **F1** | Move to next game (when in-game) |
| **F5** | Reset the game you are playing |
| **A-Z** | Type text (name input) |

## Interface Sharing

We collaborated with a group to develop the interface. Group leader: mato.urbanac@epitech.eu

## Documentation

See the `doc/` directory for:

- **library_extension_guide.md** — How to implement new graphics or game libraries compatible with the system.
- **class_diagram.md** — Visual representation of the architecture and class relationships.
- **architecture_manual.md** — Detailed explanation of how procedures are linked in the program.
