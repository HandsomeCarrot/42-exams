# 42 Exams

Personal work and notes for **42 Network** exams.

## Ranks

| Rank            | Focus                              |
| :-------------- | :--------------------------------- |
| [exam 04](./04) | C — Systems Programming            |
| [exam 05](./05) | C++ & Algorithms / Data Structures |
| [exam 06](./06) | C — Networking                     |

### exam 04

Pure C exam covering Unix process management — forking, executing binaries,
piping between processes, and file descriptor duplication with `dup2`. Also
includes recursive descent parsing: building and evaluating an Abstract Syntax
Tree for arithmetic expressions with operator precedence (`+`, `*`,
parentheses). Exercises: `ft_popen`, `vbc`.

### exam 05

Mixed C/C++ exam. The C++ side focuses on object-oriented design: inheritance,
abstract classes, Orthodox Canonical Form, operator overloading, and
container/adaptor patterns (bags, sets, BST-based containers), plus
arbitrary-precision unsigned integer arithmetic. The C side covers grid-based
algorithms: Conway's Game of Life cellular automaton and the maximal-square
dynamic programming problem. Exercises: `polyset`, `bigint`, `vect2`, `life`,
`bsq`.

### exam 06

C networking exam. A non-blocking TCP chat server bound to 127.0.0.1:
accepting clients with `select` / `poll`, assigning ids, and relaying line-based
messages between them. Exercise: `mini_serv` (both `select` and `poll` variants).

## Structure

Each rank directory contains individual exercise folders. See each rank's
`README.md` for the full exercise list, status, and implementation details.
