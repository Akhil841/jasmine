BIOS=tools/makebios.cpp
BIOSFLAGS="-o makebios"

# Compile the BIOS making program
g++ $BIOS $BIOSFLAGS
# Make the BIOS
./makebios
# Remove the BIOS making program
rm -f makebios