build/assembler test/example2/crt0.o        test/example2/crt0.s       > output.txt
build/assembler test/example2/testfile1.o   test/example2/testfile1.s  > output.txt

build/linker    test/example2/program.hex test/example2/crt0.o test/example2/testfile1.o > output.txt

build/emulator  test/example2/program.hex > output.txt