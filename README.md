# Emu8080

A library for emulating the Intel 8080 microprocessor.

## Building

Use [CMake](https://cmake.org/) to build the library and test runner.

```
$ > mkdir build && cd build
$ > cmake ..
$ > make
```

## Testing

Use ```test-runner``` to run the cpu tests found in the [test/com](test/com/) folder.

```
$ > ./build/test-runner test/com/[TEST].COM
```

## Usage

## Author

* **Ryan Kluzinski** - [rkluzinski](https://github.com/rkluzinski)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* http://altairclone.com/downloads/
    * compiled CPM binaries
    * compiled CPU tests
    * programmers manual
* https://svofski.github.io/pretty-8080-assembler/
    * for assembling 8080 asm
* http://www.shaels.net/index.php/cpm80-22-documents/cpm-bdos/31-bdos-overview
* https://www.seasip.info/Cpm/bios.html
    * reference for CPM
* http://www.xsim.com/papers/Bario.2001.emubook.pdf
    * general reference for emulation techniques
* http://pastraiser.com/cpu/i8080/i8080_opcodes.html
    * (mostly) correct summary of opcodes
* http://www.vcfed.org/forum/showthread.php?63090-Intel-8080-CPU-emulator-need-help-finding-bug(s)
    * for list of CPU errata (especially DAA)
* https://github.com/mamedev/mame/blob/master/src/devices/cpu/i8085/i8085.cpp#L767
    * for aux carry for SUB, SBB and CMP
* http://www.emulator101.com/
    * inspiration for the project
* https://github.com/begoon/i8080-core/blob/master/i8080.c
    * reference when implementing DAA
* https://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
    * parity implementation