build/assembler test/example1/crt0.o      test/example1/crt0.s     > output.txt
build/assembler test/example1/source1.o   test/example1/source1.s  > output.txt
build/assembler test/example1/source2.o   test/example1/source2.s  > output.txt

build/linker    test/example1/program.hex test/example1/crt0.o test/example1/source1.o test/example1/source2.o > output.txt

build/emulator  test/example1/program.hex > output.txt