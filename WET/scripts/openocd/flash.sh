#!/bin/bash

echo "current dir: $(pwd)"
echo "Flashing..."
output=$(openocd -f ./scripts/openocd/openocd.cfg -c "program ./build/Debug/WET.elf verify reset exit" 2>&1)

# Optionally, print the output for debugging purposes
# echo "$output"

# Check for a specific regex pattern in the output
echo "================================"
if echo "$output" | grep -q "** Programming Finished **"; then
    echo "Flash successful!"
else
    echo "$output"
    echo "================================"
    echo "FLASH FAILED!"
    exit 1
fi


