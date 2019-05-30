#!/bin/sh

g++ PuyoGameView.cpp PuyoGameMain.cpp PuyoGameController.cpp -o puyo -lncurses
./puyo
