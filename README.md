# Aradia
This is a compiler targeting x64 for TAL-0, a language from Greg Morrisett's Chapter 4 of [Advanced Topics in Types and Programming Languages](https://www.cis.upenn.edu/~bcpierce/attapl/).

## Example
Execution starts at `main`. Jumping to `_exit` will write the contents of the languages 6 general purpose registers to stdout and end the process. Check out `concept.aa`:
```
main:
  r0 = 7;
  r1 = 4;
  r2 = 0;
  jump loop

loop:
  if r0 jump _exit;
  r2 = r2 + r1;
  r0 = r0 + -1;
  jump loop
```
Compiling and running this program before piping it's output to `xxd` will yeild the following results
```
00000000: 0000 0000 0000 0000 0400 0000 0000 0000  ................
00000010: 1c00 0000 0000 0000 0000 0000 0000 0000  ................
00000020: 0000 0000 0000 0000 0000 0000 0000 0000  ................
```
As you can see, the third register `r2` has the value `0x1C`, or in other words 28 = 7 * 4.


## Planned Features
- type system
- wasm target
