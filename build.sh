#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}╔════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║                                                        ║${NC}"
echo -e "${CYAN}║        Advanced File Manager - Build Script           ║${NC}"
echo -e "${CYAN}║                                                        ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check for required files
echo -e "${YELLOW}[1/5]${NC} Checking required files..."
REQUIRED_FILES=("main.c" "api_handler.c" "api_handler.h" "mongoose.c" "mongoose.h")
MISSING_FILES=0

for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo -e "${RED}  ✗ Missing: $file${NC}"
        MISSING_FILES=1
    else
        echo -e "${GREEN}  ✓ Found: $file${NC}"
    fi
done

if [ $MISSING_FILES -eq 1 ]; then
    echo -e "${RED}Error: Missing required files. Please ensure all files are present.${NC}"
    exit 1
fi

# Check for web interface files (optional)
echo ""
echo -e "${YELLOW}[2/5]${NC} Checking web interface files..."
WEB_FILES=("index.html" "styles.css" "app.js")
WEB_COMPLETE=1

for file in "${WEB_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo -e "${YELLOW}  ⚠ Missing: $file (web interface will be limited)${NC}"
        WEB_COMPLETE=0
    else
        echo -e "${GREEN}  ✓ Found: $file${NC}"
    fi
done

# Compile the project
echo ""
echo -e "${YELLOW}[3/5]${NC} Compiling project..."
echo -e "${BLUE}  Command: gcc -o file_manager main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER${NC}"

if gcc -o file_manager main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER 2>&1 | tee /tmp/compile_output.txt; then
    echo -e "${GREEN}  ✓ Compilation successful!${NC}"
else
    echo -e "${RED}  ✗ Compilation failed!${NC}"
    echo -e "${RED}Error details:${NC}"
    cat /tmp/compile_output.txt
    exit 1
fi

# Set executable permissions
echo ""
echo -e "${YELLOW}[4/5]${NC} Setting permissions..."
chmod +x file_manager
echo -e "${GREEN}  ✓ Executable permissions set${NC}"

# Check if we should run the application
echo ""
echo -e "${YELLOW}[5/5]${NC} Build complete!"
echo ""
echo -e "${GREEN}╔════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║                                                        ║${NC}"
echo -e "${GREEN}║               Build Successful! ✓                      ║${NC}"
echo -e "${GREEN}║                                                        ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${CYAN}To run the application:${NC}"
echo -e "  ${MAGENTA}./file_manager${NC}              ${BLUE}# Terminal + Web mode${NC}"
echo ""
echo -e "${CYAN}Web interface will be available at:${NC}"
echo -e "  ${GREEN}http://localhost:8080${NC}"
echo ""

# Ask if user wants to run now
read -p "$(echo -e ${YELLOW}Do you want to run the application now? [y/N]: ${NC})" -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo -e "${GREEN}Starting application...${NC}"
    echo ""
    
    # Check OS and open browser if possible
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        (sleep 2 && xdg-open http://localhost:8080) &
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        (sleep 2 && open http://localhost:8080) &
    fi
    
    ./file_manager
else
    echo -e "${CYAN}You can run the application later with: ${MAGENTA}./file_manager${NC}"
fi
