#!/bin/bash

# Format main.c in root
[ -f main.c ] && clang-format -i main.c

# Format all .c and .h files in src/ and include/
find src include -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

echo "Formatting complete."