# 🚀 NEXUS File Manager

<div align="center">

**Advanced File Management System with Multi-Language Code Execution**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-3.0-brightgreen.svg)](VERSION)

![NEXUS Logo](https://img.shields.io/badge/NEXUS-File%20Manager-purple?style=for-the-badge&logo=data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAwIiBoZWlnaHQ9IjEwMCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48Y2lyY2xlIGN4PSI1MCIgY3k9IjUwIiByPSI0MCIgZmlsbD0iIzY2N2VlYSIvPjwvc3ZnPg==)

</div>

---

## ✨ Features

### 🎯 Core Functionality
- **Dual Interface**: Beautiful web UI + powerful terminal interface
- **File Management**: Create, edit, view, delete files with ease
- **Code Execution**: Compile and run code in 25+ programming languages
- **Directory Browser**: Navigate and select file locations visually
- **Real-time Search**: Instant file filtering
- **Auto-Save**: Location persistence across sessions

### 💻 Supported Languages

#### Compiled Languages
- **C** (`.c`) - GCC compiler
- **C++** (`.cpp`, `.cc`, `.cxx`) - G++ with C++17 support
- **Java** (`.java`) - OpenJDK/Oracle JDK
- **Go** (`.go`) - Go compiler
- **Rust** (`.rs`) - Rustc compiler
- **Swift** (`.swift`) - Swift compiler
- **Kotlin** (`.kt`) - Kotlinc compiler

#### Interpreted Languages
- **Python** (`.py`) - Python 3
- **JavaScript** (`.js`) - Node.js
- **TypeScript** (`.ts`) - TS-Node
- **Ruby** (`.rb`) - Ruby interpreter
- **PHP** (`.php`) - PHP interpreter
- **Perl** (`.pl`) - Perl interpreter
- **Lua** (`.lua`) - Lua interpreter
- **R** (`.r`, `.R`) - R language
- **Dart** (`.dart`) - Dart VM

#### Shell Scripts
- **Bash** (`.sh`, `.bash`) - Bash shell
- **Zsh** (`.zsh`) - Z shell
- **Fish** (`.fish`) - Friendly shell

### 🎨 UI Features
- **NEXUS Branding**: Custom logo and gradient design
- **Animated Background**: Dynamic gradient orbs
- **Glassmorphism**: Modern translucent effects
- **Responsive Design**: Mobile and desktop support
- **Dark Code Editor**: Syntax-friendly environment
- **Stats Dashboard**: Visual file statistics
- **Grid/List Views**: Toggle display modes

---

## 📋 Prerequisites

### Required
- **GCC** (GNU Compiler Collection)
- **pthread** library
- **POSIX-compliant system** (Linux, macOS, WSL)

### Optional (for respective languages)
- **Python 3** - for Python scripts
- **Node.js** - for JavaScript/TypeScript
- **OpenJDK** - for Java
- **G++** - for C++
- **Go** - for Go programs
- **Rust** - for Rust programs
- **Ruby, PHP, Perl, Lua, R** - for respective scripts

---

## 🔧 Installation

### Quick Install

```bash
# Download all project files
git clone <repository-url>
cd nexus-file-manager

# Make script executable
chmod +x run.sh

# Run NEXUS
./run.sh
```

The browser will automatically open to `http://localhost:8080`

### Manual Compilation

```bash
# Compile with web server support
gcc -o file_manager main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER

# Run the application
./file_manager
```

---

## 🚀 Usage

### Auto-Launch (Recommended)

```bash
./run.sh
```

This script will:
1. ✅ Compile the application
2. ✅ Check port availability
3. ✅ Automatically open your browser
4. ✅ Start the NEXUS server
5. ✅ Show real-time server logs

### Manual Launch

```bash
./file_manager
```

Then choose:
- **Option 1**: Terminal Mode
- **Option 2**: Web Mode

### Web Interface

Navigate to: `http://localhost:8080`

**Features Available:**
- 📁 Browse files in grid or list view
- ➕ Create new files with content editor
- ✏️ Edit existing files
- 👁️ View files in read-only mode
- ▶️ Execute code files (compile/run)
- 📂 Browse and select directories
- 🔍 Search files in real-time
- 📊 View file statistics

---

## 💡 How to Execute Code

### Web Interface

1. **Create/Upload your code file**
2. **Click the ▶️ Run button** on the file card
3. **Choose execution mode:**
   - **Compile** - Only compile (for compiled languages)
   - **Run** - Only run (execute existing binary)
   - **Compile & Run** - Do both in one step
4. **View output** in the execution modal

### Supported Actions by Language

| Language | Compile | Run | Compile & Run |
|----------|---------|-----|---------------|
| C/C++    | ✅      | ✅  | ✅           |
| Java     | ✅      | ✅  | ✅           |
| Go       | ✅      | ✅  | ✅           |
| Rust     | ✅      | ✅  | ✅           |
| Python   | ❌      | ✅  | ✅           |
| JavaScript| ❌     | ✅  | ✅           |
| Ruby     | ❌      | ✅  | ✅           |
| PHP      | ❌      | ✅  | ✅           |

---

## 🎯 Example: Running a C++ Program

### Step 1: Create File
```cpp
// hello.cpp
#include <iostream>
using namespace std;

int main() {
    cout << "Hello from NEXUS!" << endl;
    return 0;
}
```

### Step 2: Click "Compile & Run"

### Step 3: View Output
```
✓ Execution completed successfully!

═══════════ OUTPUT ═══════════

Hello from NEXUS!

═══════════════════════════════
Exit Code: 0
```

---

## 🛠️ Troubleshooting

### Compilation Errors

**Error: mongoose.h not found**
```bash
# Ensure all files are in the same directory
ls -la
# Should show: main.c, api_handler.c, api_handler.h, mongoose.c, mongoose.h
```

**Error: pthread library not found**
```bash
# Ubuntu/Debian
sudo apt-get install libpthread-stubs0-dev

# macOS (usually included)
# No action needed
```

### Runtime Errors

**Port 8080 already in use**
```bash
# The run.sh script automatically handles this
# Or manually kill the process
lsof -ti:8080 | xargs kill -9
```

**Browser doesn't open automatically**
- Manually navigate to: `http://localhost:8080`
- Check if your default browser is set correctly

**Code execution fails**
```bash
# Check if compiler/interpreter is installed
which gcc      # For C
which g++      # For C++
which python3  # For Python
which node     # For JavaScript
which javac    # For Java
```

**File permissions issues**
```bash
# Make scripts executable
chmod +x run.sh
chmod +x file_manager
```

---

## 📂 Project Structure

```
nexus-file-manager/
│
├── main.c              # Main application logic
├── api_handler.c       # Backend API implementation
├── api_handler.h       # API header file
├── mongoose.c          # Web server library
├── mongoose.h          # Web server header
│
├── index.html          # Web interface (NEXUS branded)
├── style.css           # Modern CSS with animations
├── app.js              # Frontend JavaScript
│
├── run.sh              # Auto-launch script ⭐
├── build.sh            # Build script
├── localhostRun.sh     # Alternative launch script
│
└── README.md           # This file
```

---

## 🎨 Customization

### Change Default Port

Edit `run.sh`:
```bash
PORT=8080  # Change to your desired port
```

### Modify Theme Colors

Edit `style.css`:
```css
:root {
    --primary: #667eea;        /* NEXUS purple */
    --secondary: #764ba2;      /* NEXUS gradient */
    /* Add your colors here */
}
```

### Add More Language Support

Edit `api_handler.c` and add to `handle_execute_file()`:
```c
else if (strcmp(ext, "your_ext") == 0) {
    snprintf(command, sizeof(command), "your_compiler %s 2>&1", filepath);
}
```

---

## 🔐 Security Notes

- ⚠️ **Local Use Only**: Web server binds to `0.0.0.0` for accessibility
- ⚠️ **No Authentication**: Designed for local development
- ⚠️ **Code Execution**: Runs with current user permissions
- ⚠️ **Input Validation**: Be cautious with untrusted code

### Production Use
If deploying to production:
1. Add authentication
2. Implement HTTPS
3. Sanitize all inputs
4. Use sandboxed execution
5. Add rate limiting

---

## 📊 Performance

- **File Operations**: Nearly instant (<100ms)
- **Code Compilation**: Depends on code size
  - Small programs: 1-2 seconds
  - Large projects: 5-10 seconds
- **Web Interface**: Smooth 60 FPS animations
- **Memory Usage**: ~10-20MB (idle)

---

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

- [ ] Add syntax highlighting in code editor
- [ ] Implement file upload functionality
- [ ] Add project/workspace support
- [ ] Integrate version control (Git)
- [ ] Add collaborative editing
- [ ] Implement file compression
- [ ] Add terminal emulator
- [ ] Support for more languages

---

## 📝 Changelog

### Version 3.0 (Current)
- ✨ Added NEXUS branding with custom logo
- 🎨 Enhanced UI with gradient animations
- 🚀 Auto-launch script with browser opening
- 💻 Support for 25+ programming languages
- 🔧 Fixed code execution issues
- 📊 Added detailed execution output
- 🎯 Improved error handling

### Version 2.0
- Added web interface
- Implemented REST API
- Added directory browser

### Version 1.0
- Terminal-only version
- Basic file operations

---

## 📜 License

MIT License - feel free to use, modify, and distribute.

---

## 🙏 Acknowledgments

- **Mongoose** - Embedded web server library
- **Inter Font** - Modern typography
- **Material Design** - UI/UX principles

---

## 📧 Support

For issues, questions, or suggestions:
1. Check the troubleshooting section
2. Review the code documentation
3. Create an issue on GitHub

---

<div align="center">

**Made with ❤️ by Jatin Sharma**

⭐ **Star this repo if you find it useful!** ⭐

[![GitHub](https://img.shields.io/badge/GitHub-Follow-black?style=social&logo=github)](https://github.com)

</div>

---

## 🎯 Quick Commands Reference

```bash
# Compile and run
./run.sh

# Just compile
gcc -o file_manager main.c api_handler.c mongoose.c -lpthread -DENABLE_WEB_SERVER

# Run without auto-launch
./file_manager

# Make scripts executable
chmod +x *.sh

# Clean compiled files
rm -f file_manager *.out *.class *.jar

# Check if port is in use
lsof -i :8080

# Kill process on port 8080
lsof -ti:8080 | xargs kill -9
```

---

**Enjoy using NEXUS File Manager! 🚀**
