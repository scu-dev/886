<p align="center"><img alt="886 Logo Banner" width="512" src="icons/886_banner.png" /></p>

<h1 align="center">886</h1>

886 - a REPL interpreter of 8086/8088 assembly, using MASM style.

Built withOUT performance in mind. Don't use it for real programs.

Built NOT following any internal standard or mechanisms. Please don't learn how 8086/8088 works from the source code.

## OK, but why?

Because the assembly course in [Nichuan](https://www.scu.edu.cn) is developed in the 1980s and had not been updated since.

If we want to literally do any experiments, you need an tutorial with TREE(3) steps to set up DOSBox on Windows 11 and download the really appropriate ***king version of MASM that can run on 32-bit DOS and assemble to 16-bit programs.

Yes, before you ask, you can't use NASM or any other assembler that's simply better, because again, the assembly course in [Nichuan](https://www.scu.edu.cn) is developed in the 1980s and had not been updated since, so we have to memorize MASM style code to pass in the exam.

So I made this thing to make our lives a little bit easier.

## OK, so why this name?

I want to say goodbye to this course ASAP.

## Usage

### REPL Mode

Double click the executable to use the REPL mode.

There are of course some debug and I/O commands in the REPL mode. Every custom command starts with an underscore `_`:

| Command |         Arguments         |                Description                |
| :-----: | :-----------------------: | :---------------------------------------: |
|  `__`   |             /             |    Start or finish a multi-line input.    |
| `_hlp`  |             /             |            Print a short help.            |
| `_prr`  |             /             | Print the current bits in every register. |
| `_prs`  | Stack segment declaration |         Print that stack segment.         |
| `_prd`  | Data segment declaration  |         Print that data segment.          |
| `_prm`  |   `WORD PTR` Start, End   |          Print the memory range.          |
|

### Argument Table

|     Argument     |  Type   |               Description               |   Default   |
| :--------------: | :-----: | :-------------------------------------: | :---------: |
| `-x`, `--infix`  | String  | Calculate a constant infix expression.  |     N/A     |
|  `-f`, `--file`  |  Path   |        Execute an assembly file.        |     N/A     |
| `-m`, `--memory` | Number  | Total memory space (magnitude in bytes) | `10` (1 MB) |
|  `-s`, `--step`  | Boolean |            Enable step debug            |   `false`   |

## Legal

©2025 [LJM12914](https://github.com/ljm12914). Licensed under the [Apache-2.0 license](https://github.com/scu-dev/886/blob/main/LICENSE).
