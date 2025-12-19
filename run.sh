#!/bin/bash

# NEXUS File Manager - Auto Launch Script
# =========================================

APP_NAME="file_manager"
PORT=8080
URL="http://localhost:$PORT"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Clear screen and show banner
clear
echo -e "${CYAN}"
echo "╔══════════════════════════════════════════════════════════╗"
echo "║                                                          ║"
echo "║              🚀  NEXUS FILE MANAGER  🚀                  ║"
echo "║                  Auto-Launch Script                      ║"
echo "║                                                          ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo -e "${NC}"

# Step 1: Compilation
echo -e "${YELLOW}[1/4]${NC} ${WHITE}Compiling NEXUS...${NC}"
echo ""

if [ -f "api_handler.c" ] && [ -f "main.c" ]; then
    gcc -o $APP_NAME main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER 2>&1 | \
        sed "s/^/    ${BLUE}│${NC} /"
else
    gcc -o $APP_NAME main.c mongoose.c -lpthread -DENABLE_WEB_SERVER 2>&1 | \
        sed "s/^/    ${BLUE}│${NC} /"
fi

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo ""
    echo -e "${RED}✗ Compilation failed!${NC}"
    echo -e "${RED}Please check the errors above and try again.${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}✓ Compilation successful!${NC}"
sleep 1

# Step 2: Check if port is available
echo ""
echo -e "${YELLOW}[2/4]${NC} ${WHITE}Checking port ${PORT}...${NC}"

if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null 2>&1 ; then
    echo -e "${YELLOW}⚠  Port $PORT is already in use${NC}"
    echo -e "${YELLOW}   Attempting to free the port...${NC}"
    
    # Try to kill the process
    PID=$(lsof -ti:$PORT)
    if [ ! -z "$PID" ]; then
        kill -9 $PID 2>/dev/null
        sleep 1
    fi
    
    if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null 2>&1 ; then
        echo -e "${RED}✗ Could not free port $PORT${NC}"
        echo -e "${YELLOW}   Please manually stop the process using port $PORT${NC}"
        exit 1
    else
        echo -e "${GREEN}✓ Port freed successfully${NC}"
    fi
else
    echo -e "${GREEN}✓ Port $PORT is available${NC}"
fi

sleep 1

# Step 3: Prepare browser launch
echo ""
echo -e "${YELLOW}[3/4]${NC} ${WHITE}Preparing browser launch...${NC}"

# Function to open browser
open_browser() {
    sleep 2
    
    # Show waiting animation
    echo -e "${CYAN}    ⏳ Waiting for server to start...${NC}"
    
    # Wait for server to be ready (max 10 seconds)
    for i in {1..20}; do
        if curl -s $URL > /dev/null 2>&1; then
            break
        fi
        sleep 0.5
    done
    
    echo -e "${GREEN}    ✓ Server is ready!${NC}"
    echo -e "${MAGENTA}    🌐 Opening browser...${NC}"
    
    # Detect OS and open browser accordingly
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux
        if command -v xdg-open > /dev/null; then
            xdg-open $URL 2>/dev/null &
        elif command -v gnome-open > /dev/null; then
            gnome-open $URL 2>/dev/null &
        elif command -v kde-open > /dev/null; then
            kde-open $URL 2>/dev/null &
        else
            echo -e "${YELLOW}    Could not detect browser. Please open: ${WHITE}$URL${NC}"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        open $URL 2>/dev/null &
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        # Windows (Git Bash, WSL, etc.)
        if command -v start > /dev/null; then
            start $URL 2>/dev/null &
        elif command -v cmd.exe > /dev/null; then
            cmd.exe /c start $URL 2>/dev/null &
        else
            echo -e "${YELLOW}    Could not detect browser. Please open: ${WHITE}$URL${NC}"
        fi
    else
        echo -e "${YELLOW}    Unknown OS. Please manually open: ${WHITE}$URL${NC}"
    fi
}

# Start browser opening in background
open_browser &
BROWSER_PID=$!

echo -e "${GREEN}✓ Browser launch initiated${NC}"
sleep 1

# Step 4: Start the application
echo ""
echo -e "${YELLOW}[4/4]${NC} ${WHITE}Starting NEXUS Server...${NC}"
echo ""
echo -e "${GREEN}╔══════════════════════════════════════════════════════════╗"
echo -e "║                                                          ║"
echo -e "║              ✨  NEXUS IS NOW RUNNING  ✨                ║"
echo -e "║                                                          ║"
echo -e "║  ${WHITE}Web Interface:${GREEN}  ${WHITE}$URL${GREEN}                   ║"
echo -e "║  ${WHITE}Status:${GREEN}         Ready and listening                     ║"
echo -e "║                                                          ║"
echo -e "║  ${YELLOW}Press Ctrl+C to stop the server${GREEN}                      ║"
echo -e "║                                                          ║"
echo -e "╚══════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${CYAN}─────────────────────────────────────────────────────────────${NC}"
echo -e "${WHITE}Server logs will appear below:${NC}"
echo -e "${CYAN}─────────────────────────────────────────────────────────────${NC}"
echo ""

# Handle cleanup on exit
cleanup() {
    echo ""
    echo -e "${YELLOW}Shutting down NEXUS...${NC}"
    kill $BROWSER_PID 2>/dev/null
    echo -e "${GREEN}✓ NEXUS stopped successfully${NC}"
    echo -e "${CYAN}Thank you for using NEXUS! 👋${NC}"
    exit 0
}

trap cleanup SIGINT SIGTERM

# Run the application
./$APP_NAME

# If we get here, the app exited normally
cleanup
