# connect4-ai

This repository contains a Connect 4 game AI developed for the CodinGame platform. The AI is implemented in C++ and achieves a performance level equivalent to the Wood 1 league, currently ranked 69th as of July 3rd, 2023.

## Algorithm

The AI uses the Monte Carlo Tree Search (MCTS) algorithm. MCTS is a heuristic search algorithm that combines random sampling and tree search to explore large game trees efficiently. By simulating multiple random games from the current game state and using the results to guide decision-making, MCTS can make informed decisions even in complex game scenarios.

## Usage

To use this AI in your projects, follow these steps:

1. Clone the repository:

```shell
git clone https://github.com/sh-mug/connect4-ai.git
```

2. Compile the code:

```shell
g++ -o connect4-ai -O2 main.cpp -lpthread
```

3. Run the AI:

```shell
./connect4-ai
```

## License

This project is licensed under the [MIT License](LICENSE.md).

## Acknowledgements

I would like to acknowledge the CodinGame platform for providing the environment to develop and test this Connect 4 AI.
