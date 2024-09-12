# make build directories
mkdir build
mkdir build/object

# make objects
gcc -c -o build/object/main.o src/main.c -lcurl
gcc -c -o build/object/discord.o src/discord.c -lcurl
gcc -c -o build/object/ping.o src/ping.c -lcurl
gcc -c -o build/object/strsplit.o src/strsplit.c -lcurl

# link objects
gcc build/object/*.o -o build/main -lcurl

# run
build/main src/settings.txt
