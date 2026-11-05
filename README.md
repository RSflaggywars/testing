# todo-cli

A simple command-line to-do list manager written in C. Tasks are stored
in a plain-text `todo.txt` file, so you can edit them by hand if you like.

## Build

Requires a C compiler (gcc, clang, or MSVC).

```sh
make        # builds ./todo
# or on Windows without make:
gcc -Wall -Wextra -std=c11 -O2 -o todo todo.c
```

## Usage

```
todo add "buy milk"    # add a task
todo list              # list all tasks
todo done 2            # mark task #2 as done
todo del 1             # delete task #1
todo clear             # delete all tasks
```

Tasks are numbered in the order they appear. Completed tasks show `[x]`.

## Example

```
$ ./todo add buy milk
todo add buy milk
Added: buy milk
$ ./todo add "finish report"
$ ./todo list
 1. [ ] buy milk
 2. [ ] finish report
$ ./todo done 1
Completed task 1.
$ ./todo list
 1. [x] buy milk
 2. [ ] finish report
```

## License

MIT
