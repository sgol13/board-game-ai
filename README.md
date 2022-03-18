# Board game AI
This is an implementation of a simple board game which allows a user to play against a computer. The rules of the game are simple:
* Each player has three pieces (`A` and `B`). They can move any number of squares vertically, horizontally or diagonally but cannot jump over or stand on obstacles (`#`) and other player's pieces.
* The aim of the game is to put all three pieces on opponent's initial positions.

The AI algorithm, which the computer uses to choose optimal moves, is based on a minimax rule. My main motivation for creating this project was the fact that I wanted to do some experiments with the algorithm, setting various parameters affecting its behaviour and observing the decisions made in edge cases.


<a href="https://youtu.be/ps3A60N3758">
         <img alt="Board Game AI video" src="https://i.postimg.cc/FKXTkYHw/play-screen.png" width="400">
</a>                                                                                                     

## Compilation

```
git clone https://github.com/sgol13/board-game-ai.git
cd board-game-ai
g++ -o game main.cpp
./game
```

## License
This project is under MIT [license](LICENSE).
