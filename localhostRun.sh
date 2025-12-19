#!/bin/bash

# Define the binary name
APP_NAME="file_manager"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}╔════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║   🚀 Quick Build & Launch Script 🚀       ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════╝${NC}"
echo ""

echo -e "${YELLOW}--- 🛠️  Phase 1: Compiling ---${NC}"

# Check if we have the new separated files
if [ -f "api_handler.c" ] && [ -f "main.c" ]; then
    echo -e "${CYAN}Detected new project structure${NC}"
    gcc -o $APP_NAME main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER
else
    echo -e "${CYAN}Using original main.c${NC}"
    gcc -o $APP_NAME main.c mongoose.c -lpthread -DENABLE_WEB_SERVER
fi

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ Compilation successful.${NC}"
    
    echo ""
    echo -e "${YELLOW}--- 🌐 Phase 2: Launching Browser ---${NC}"
    
    # Opens browser in background so the terminal stays active
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        (sleep 2 && xdg-open http://localhost:8080 &> /dev/null) &
        echo -e "${GREEN}Browser will open in 2 seconds...${NC}"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        (sleep 2 && open http://localhost:8080) &
        echo -e "${GREEN}Browser will open in 2 seconds...${NC}"
    else
        echo -e "${CYAN}Please manually open: http://localhost:8080${NC}"
    fi

    echo ""
    echo -e "${YELLOW}--- 🚀 Phase 3: Running Application ---${NC}"
    echo -e "${GREEN}Starting File Manager...${NC}"
    echo ""
    
    ./$APP_NAME
else
    echo -e "${RED}❌ Error: Compilation failed.${NC}"
    echo -e "${RED}Check your source files:${NC}"
    if [ -f "main.c" ]; then
        echo -e "  - main.c"
        echo -e "  - api_handler.c"
        echo -e "  - api_handler.h"
    else
        echo -e "  - main.c"
    fi
    echo -e "  - mongoose.c"
    echo -e "  - mongoose.h"
    exit 1
fi
