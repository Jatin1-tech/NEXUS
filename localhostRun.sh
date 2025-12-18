#!/bin/bash

# Define the binary name
APP_NAME="file_manager"

echo "--- 🛠️  Phase 1: Compiling ---"
# Using your specific command
gcc -o $APP_NAME main.c mongoose.c -lpthread -DENABLE_WEB_SERVER

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful."
    
    echo "--- 🌐 Phase 2: Launching Browser ---"
    # Opens browser in background so the terminal stays active
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        xdg-open http://localhost:8080 &> /dev/null &
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        open http://localhost:8080
    fi

    echo "--- 🚀 Phase 3: Running Application ---"
    ./$APP_NAME
else
    echo "❌ Error: Compilation failed. Check your main.c or mongoose.c files."
fi
