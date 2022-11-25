#  Plumbing utils Makefile
#   ________   ___        ___  ___   _____ ______    ________   ___   ________    ________
#  |\   __  \ |\  \      |\  \|\  \ |\   _ \  _   \ |\   __  \ |\  \ |\   ___  \ |\   ____\
#  \ \  \|\  \\ \  \     \ \  \\\  \\ \  \\\__\ \  \\ \  \|\ /_\ \  \\ \  \\ \  \\ \  \___|
#   \ \   ____\\ \  \     \ \  \\\  \\ \  \\|__| \  \\ \   __  \\ \  \\ \  \\ \  \\ \  \  ___
#    \ \  \___| \ \  \____ \ \  \\\  \\ \  \    \ \  \\ \  \|\  \\ \  \\ \  \\ \  \\ \  \|\  \
#     \ \__\     \ \_______\\ \_______\\ \__\    \ \__\\ \_______\\ \__\\ \__\\ \__\\ \_______\
#      \|__|      \|_______| \|_______| \|__|     \|__| \|_______| \|__| \|__| \|__| \|_______|
#   ___  ___   _________   ___   ___        ________
#  |\  \|\  \ |\___   ___\|\  \ |\  \      |\   ____\
#  \ \  \\\  \\|___ \  \_|\ \  \\ \  \     \ \  \___|_
#   \ \  \\\  \    \ \  \  \ \  \\ \  \     \ \_____  \
#    \ \  \\\  \    \ \  \  \ \  \\ \  \____ \|____|\  \
#     \ \_______\    \ \__\  \ \__\\ \_______\ ____\_\  \
#      \|_______|     \|__|   \|__| \|_______||\_________\
#                                             \|_________|
#  
#  Plumbing utilities for GNU/Linux.
#  https://github.com/fontseca/plumbing-utils
#  
#  Copyright (C) 2022 by Jeremy Fonseca <fonseca.dev@outlook.com>
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

CC := gcc
CFLAGS := -g -Wall -Wextra -Werror -pedantic
OBJ := $(patsubst %.c,%.o, $(wildcard *.c))

jst: jst.c
	$(CC) -o $@ $(CFLAGS) $?
