#!/bin/sh

g++ -std=c++03 PuyoGameView.cpp PuyoGame.cpp PGController.cpp -o puyo -lncurses
./puyo
