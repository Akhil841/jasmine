BIOS=tools/makebios.cpp
BIOSFLAGS="-std=c++17 -o makebios"

# Compile the BIOS making program
g++ $BIOS $BIOSFLAGS
# Make the BIOS
./makebios
# Remove the BIOS making program
rm -f makebios