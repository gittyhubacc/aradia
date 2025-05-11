# Aradia
This is a compiler targeting linux x64 for a superset of TAL-0,
a language from Greg Morrisett's Chapter 4 of 
[Advanced Topics in Types and Programming Languages](https://www.cis.upenn.edu/~bcpierce/attapl/).

## Building
Run `make` in the repository root to compile the aradia compiler `bin/aradiac`.
`aradiac` reads aradia source from stdin and writes nasm-flavor assembly to stdout.
For example `cat source.aa | ./bin/aradiac > output.s`. 
See the makefile for more examples / usages.

## Examples
Examples can be built via `make {simple,concept,hello}`, which'll produce binaries in the `bin` folder next to `aradiac`.

### concept.aa
```
main:	r0 = 7;
	r1 = 4;
	r2 = 0;		
	jump loop

loop:	if r0 jump done;
	r2 = r2 + r1;
	r0 = r0 + -1;
	jump loop

done:	emit r2;
	jump _exit
```
Everything does what you are probably assuming it does.
Note that the emit instruction writes the entire 64bit register to stdout.

```
make concept && ./bin/concept | xxd
```
Running the above produces the following output (sans the make/build output)

```
00000000: 1c00 0000 0000 0000                      ........
```

### simple.aa
```
main:
	r0 = 47;
	r1 = 47;
	r1 = r1 + r0;
	emit r1;
	jump _exit
```
To build:
```
make simple && ./bin/simple | xxd
```
And you get:
```
00000000: 5e00 0000 0000 0000                      ^.......
```

### hello_world.aa
```
main:	r0 = 6278066737626506568;	# 0x57202C6F6C6C6548 
	r1 = 11138535027311; 		# 0x00000A21646C726F
	emit r0;
	emit r1;
	jump _exit
```
To build:
```
make hello && ./bin/hello | xxd
```
And you get:
```
00000000: 4865 6c6c 6f2c 2057 6f72 6c64 210a 0000  Hello, World!...
```

## Planned Features
- type system
- hexadecimal literals
- wasm target
