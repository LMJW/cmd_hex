# This is a command line based hex game

[wiki](https://en.wikipedia.org/wiki/Hex_(board_game))

The computer player is simulated use monte carlo simulation. You can register
blue player or red player in main function.

To compile the program, you can run `g++ -std=c++11 -pipe -O2 hex.cc -o hex`.

Then you can execute `./hex` in command line to run this program.

This is the program interface...
```
Game started. Press Ctrl+C if you want to quit game
the grid is a 11x11 grid.
The index starts from up left corner with index 0,0, and the down right corner with index 10,10.
You need to enter the index r c seperate with space to place your hex on the grid
. - . - . - . - . - . - . - . - . - . - .
 \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
  . - . - . - . - . - . - . - . - . - . - .
   \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
    . - . - . - . - . - . - . - . - . - . - .
     \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
      . - . - . - . - . - . - . - . - . - . - .
       \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
        . - . - . - . - . - . - . - . - . - . - .
         \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
          . - . - . - . - . - . - . - . - . - . - .
           \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
            . - . - . - . - . - . - . - . - . - . - .
             \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
              . - . - . - . - . - . - . - . - . - . - .
               \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
                . - . - . - . - . - . - . - . - . - . - .
                 \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
                  . - . - . - . - . - . - . - . - . - . - .
                   \ / \ / \ / \ / \ / \ / \ / \ / \ / \ / \
                    . - . - . - . - . - . - . - . - . - . - .
Now is Blue player's turn:
```