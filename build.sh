set -xe

g++ -Wextra -Wall ./src/asm.cpp -O3 -o ./bin/asm
g++ -Wextra -Wall ./src/emu.cpp -O3 -o ./bin/emu