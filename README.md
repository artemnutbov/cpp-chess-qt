# ♟️ Qt C++ Chess Engine

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.0%2B-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

The game can be played against an AI bot and locally against a friend. In the start menu, you can choose which side to play as White or as Black.

<p align="center">
  <img src="https://github.com/user-attachments/assets/bdd26238-daa5-4694-adfd-bb45cfdc4564" alt="Gameplay GIF" width="600" />
</p>

## Game features:

* **Legal Move Highlighting:** Click any piece on the board to see where it is allowed to go.
* **Display Captured Pieces:** Tracks and displays captured pieces above and below the board.
* **Undo Move:** Undo your last move and opponent's last move.
* **Game Over & Restart:** Shows how the game ended, and after that, a new game button appears.
* **Standard Rules:** Supports all rules, including [Castling](https://en.wikipedia.org/wiki/Castling), [En Passant](https://en.wikipedia.org/wiki/En_passant), and [Pawn Promotion](https://en.wikipedia.org/wiki/Promotion_(chess)).
<p align="center">
  <img src="https://github.com/user-attachments/assets/566afde9-e49a-4ed7-a8a7-33871bad3dd2" alt="Gameplay GIF" width="600" />
</p>

* **Draw Detection:** Automatically recognizes [Stalemates](https://en.wikipedia.org/wiki/Stalemate), the [50-Move Rule](https://en.wikipedia.org/wiki/Fifty-move_rule), and [3-Fold Repetition](https://en.wikipedia.org/wiki/Threefold_repetition).
  
<p align="center">
  <img src="https://github.com/user-attachments/assets/90962519-61ff-4a2b-bfa5-09b2aa31e1c7" alt="Gameplay GIF" width="600" />
</p>

## About AI Engine 


The bot is capable of competing with ~2000 Elo level bots on Chess.com in informal testing. It uses [Negamax](https://www.chessprogramming.org/Negamax)(a simple version of Minimax) and [Alpha-Beta Pruning](https://www.chessprogramming.org/Alpha-Beta) to look ahead and evaluate the best possible moves.
Also, the engine uses [Iterative Deepening](https://www.chessprogramming.org/Iterative_Deepening), it searches deeper and deeper into the move tree until its time limit runs out, ensuring it plays a strong move in a reasonable amount of time.
And to spot 3-fold repetitions, it uses [Zobrist Hashing](https://www.chessprogramming.org/Zobrist_Hashing).

## Note About Architecture

I originally built the board using OOP(an abstract `Basic_figure` class and a 2D array of `unique_ptr`s). But after deeper research into how chess AI algorithms work, I refactored the whole thing into a flat **1D array using `enum class`** to speed up calculation(after refactoring don't need to allocate and deallocate 
memory in heap, just assigning). My chess logic is separated from the Qt UI code. Also, I use Qt's `QThreadPool` to calculate the bot's moves in the background, so the game window never freezes while the AI is "thinking".

Additionally, I use **GoogleTest** to automatically check that the core game logic works correctly. And follow the **Google C++ Style Guide** for naming, and formatting using `clang-format`.

## Build

To build and run this project, you'll need:

* **C++ Compiler:** Supporting C++17. Works correctly on GCC, Clang, and MinGW-w64; MSVC (not tested).
* **Qt:** Qt 5 or Qt 6 (Components: `Widgets`, `Concurrent`).
* **Libraries:** Boost. GoogleTest (GTest) will fetch automatically during the first build via CMake 'FetchContent'.
* **CMake:** Version 3.16 or higher. 


<details>
  <summary><b>Click here for Build Instructions</b></summary>
  
  <br>
  
  1. **Clone the repository:**
  ```
  git clone https://github.com/artemnutbov/cpp-chess-qt
  cd cpp-chess-qt
  ```
  
  2. **Compile and Run:**
  
  *Linux*
    
  ```
  mkdir build && cd build
  cmake .. 
  cmake --build .
  ./my_chess_game
  ```

  *Windows*
  
  Important note!!! By default, BOOST_ROOT is set to `C:/Libraries/boost_1_89_0` you can change it in CMakelists.txt `set(BOOST_ROOT "C:/Libraries/boost_1_89_0")`.   
  ```
  mkdir build && cd build
  cmake -DBOOST_ROOT="C:/Libraries/boost_1_89_0" ..
  cmake --build .
  ```
  
</details>

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
