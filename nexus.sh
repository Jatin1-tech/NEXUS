#!/bin/bash

# ╔══════════════════════════════════════════════════════════╗
# ║                  NEXUS FILE MANAGER                      ║
# ║                  Universal Launcher                      ║
# ╚══════════════════════════════════════════════════════════╝

APP_NAME="file_manager"
PORT=8080

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m'

# Function to show banner
show_banner() {
    clear
    echo -e "${CYAN}"
    cat << "EOF"
    ███╗   ██╗███████╗██╗  ██╗██╗   ██╗███████╗
    ████╗  ██║██╔════╝╚██╗██╔╝██║   ██║██╔════╝
    ██╔██╗ ██║█████╗   ╚███╔╝ ██║   ██║███████╗
    ██║╚██╗██║██╔══╝   ██╔██╗ ██║   ██║╚════██║
    ██║ ╚████║███████╗██╔╝ ██╗╚██████╔╝███████║
    ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝
                                                
         Advanced File Manager & Code Editor    
EOF
    echo -e "${NC}"
    echo -e "${WHITE}         Version 3.0 - By Jatin Sharma${NC}"
    echo ""
}

# Function to check requirements
check_requirements() {
    echo -e "${YELLOW}Checking requirements...${NC}"
    
    local all_good=true
    
    # Check GCC
    if ! command -v gcc &> /dev/null; then
        echo -e "${RED}✗ GCC not found${NC}"
        all_good=false
    else
        echo -e "${GREEN}✓ GCC found${NC}"
    fi
    
    # Check files
    local required_files=("main.c" "mongoose.c" "mongoose.h" "index.html" "style.css" "app.js")
    for file in "${required_files[@]}"; do
        if [ ! -f "$file" ]; then
            echo -e "${RED}✗ Missing: $file${NC}"
            all_good=false
        fi
    done
    
    if [ "$all_good" = true ]; then
        echo -e "${GREEN}✓ All requirements met${NC}"
        return 0
    else
        echo -e "${RED}✗ Some requirements are missing${NC}"
        return 1
    fi
}

# Function to compile
compile_app() {
    echo ""
    echo -e "${YELLOW}Compiling NEXUS...${NC}"
    
    if [ -f "api_handler.c" ]; then
        gcc -o $APP_NAME main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER 2>&1
    else
        gcc -o $APP_NAME main.c mongoose.c -lpthread -DENABLE_WEB_SERVER 2>&1
    fi
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Compilation successful${NC}"
        return 0
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        return 1
    fi
}

# Function to open browser
open_browser() {
    local url="http://localhost:$PORT"
    
    # Wait for server
    echo -e "${CYAN}Waiting for server...${NC}"
    for i in {1..20}; do
        if curl -s $url > /dev/null 2>&1; then
            break
        fi
        sleep 0.5
    done
    
    echo -e "${GREEN}✓ Server ready${NC}"
    echo -e "${MAGENTA}🌐 Opening browser...${NC}"
    
    # Open based on OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        xdg-open $url 2>/dev/null || gnome-open $url 2>/dev/null || kde-open $url 2>/dev/null &
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        open $url 2>/dev/null &
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        start $url 2>/dev/null || cmd.exe /c start $url 2>/dev/null &
    fi
}

# Function to kill existing process
kill_existing() {
    if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null 2>&1 ; then
        echo -e "${YELLOW}Stopping existing server...${NC}"
        lsof -ti:$PORT | xargs kill -9 2>/dev/null
        sleep 1
        echo -e "${GREEN}✓ Stopped${NC}"
    fi
}

# Main menu
show_menu() {
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${WHITE}Select an option:${NC}"
    echo ""
    echo -e "  ${GREEN}[1]${NC} 🚀 Quick Launch (Auto-open browser)"
    echo -e "  ${GREEN}[2]${NC} 💻 Terminal Mode Only"
    echo -e "  ${GREEN}[3]${NC} 🌐 Web Mode (Manual open)"
    echo -e "  ${GREEN}[4]${NC} 🔨 Rebuild Application"
    echo -e "  ${GREEN}[5]${NC} 📊 Check Requirements"
    echo -e "  ${GREEN}[6]${NC} 🔧 Kill Existing Server"
    echo -e "  ${GREEN}[0]${NC} ❌ Exit"
    echo ""
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -n -e "${YELLOW}Enter choice [0-6]: ${NC}"
}

# Cleanup function
cleanup() {
    echo ""
    echo -e "${YELLOW}Shutting down...${NC}"
    echo -e "${GREEN}✓ NEXUS stopped${NC}"
    echo -e "${CYAN}Thank you for using NEXUS! 👋${NC}"
    exit 0
}

trap cleanup SIGINT SIGTERM

# Main program
main() {
    show_banner
    
    # Check if already compiled
    if [ ! -f "$APP_NAME" ]; then
        echo -e "${YELLOW}First time setup...${NC}"
        if ! check_requirements; then
            echo ""
            echo -e "${RED}Please install missing requirements and try again${NC}"
            exit 1
        fi
        
        if ! compile_app; then
            exit 1
        fi
    fi
    
    while true; do
        echo ""
        show_menu
        read -r choice
        
        case $choice in
            1)  # Quick Launch
                echo ""
                echo -e "${GREEN}Starting NEXUS in Quick Launch mode...${NC}"
                kill_existing
                
                # Start browser opener in background
                (sleep 2 && open_browser) &
                
                echo ""
                echo -e "${GREEN}╔════════════════════════════════════════════════╗${NC}"
                echo -e "${GREEN}║                                                ║${NC}"
                echo -e "${GREEN}║        🚀 NEXUS IS NOW RUNNING 🚀              ║${NC}"
                echo -e "${GREEN}║                                                ║${NC}"
                echo -e "${GREEN}║  URL: ${WHITE}http://localhost:$PORT${GREEN}                   ║${NC}"
                echo -e "${GREEN}║                                                ║${NC}"
                echo -e "${GREEN}║  ${YELLOW}Press Ctrl+C to stop${GREEN}                       ║${NC}"
                echo -e "${GREEN}╚════════════════════════════════════════════════╝${NC}"
                echo ""
                
                ./$APP_NAME
                cleanup
                ;;
                
            2)  # Terminal Mode
                echo ""
                echo -e "${GREEN}Starting Terminal Mode...${NC}"
                ./$APP_NAME
                ;;
                
            3)  # Web Mode
                echo ""
                echo -e "${GREEN}Starting Web Mode...${NC}"
                kill_existing
                echo -e "${CYAN}Please open: ${WHITE}http://localhost:$PORT${NC}"
                echo -e "${YELLOW}Press Ctrl+C to stop${NC}"
                echo ""
                ./$APP_NAME
                cleanup
                ;;
                
            4)  # Rebuild
                if check_requirements && compile_app; then
                    echo -e "${GREEN}✓ Rebuild complete${NC}"
                fi
                ;;
                
            5)  # Check Requirements
                check_requirements
                ;;
                
            6)  # Kill Server
                kill_existing
                ;;
                
            0)  # Exit
                echo -e "${CYAN}Goodbye! 👋${NC}"
                exit 0
                ;;
                
            *)
                echo -e "${RED}Invalid choice. Please try again.${NC}"
                ;;
        esac
        
        if [ "$choice" != "1" ] && [ "$choice" != "2" ] && [ "$choice" != "3" ]; then
            echo ""
            read -p "Press Enter to continue..."
            show_banner
        fi
    done
}

# Run main program
main
