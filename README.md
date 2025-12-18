# NEXUS
## “A native local system that connects files, execution, terminal, and web.”

> Created by **Jatin Sharma**  
> Status: 🚧 Development Underway  
> Contributions, ideas, and improvements are highly appreciated.

---

## Overview

This project is a **native, local-first file manager and code interaction environment**
built using **C** as the core language and an **embedded HTTP server** for optional web access.

The goal of this project is not to compete with existing heavyweight IDEs or cloud-based
tools, but to explore a **low-level, transparent, fast, and hackable** system that allows:

- Managing files locally
- Editing files via terminal and browser
- Inspecting file existence and content
- Executing supported files natively
- Viewing execution output in terminal or browser

This project is intentionally designed to stay **close to the operating system**.

---

## Philosophy

This project follows a few strong principles:

1. **Local-first**
   - No cloud dependency
   - No telemetry
   - No background services beyond what the user starts

2. **Native core**
   - Written in C
   - Minimal abstraction layers
   - Direct OS interaction

3. **Dual interface**
   - Terminal (CLI) is first-class
   - Web interface is optional and additive

4. **Transparency**
   - No hidden processes
   - No magic execution
   - User explicitly controls actions

5. **Hackability**
   - Easy to modify
   - Easy to extend
   - Designed for learning and experimentation

---

## What This Project Is

- A **file manager**
- A **code editor**
- A **local execution interface**
- A **systems programming experiment**
- A **foundation for future tooling**

---

## What This Project Is NOT

- Not a replacement for VS Code
- Not a cloud IDE
- Not production-hardened (yet)
- Not sandboxed by default (execution is controlled but not isolated)

---

## Current Features

### Terminal Interface (CLI)

- Create files
- Edit files line-by-line
- View file contents
- ANSI-colored UI
- Simple menu-based navigation
- Optional future support for arrow-key navigation

### Web Interface (Optional)

- Embedded HTTP server (Mongoose)
- Browser-based UI
- List files
- View file contents
- Edit files
- Create new files
- Runs locally on `localhost`

### Core Capabilities

- Shared state between CLI and Web
- Conditional compilation (with or without web support)
- Cross-platform intent (Linux-first, Windows planned)
- Minimal external dependencies

---

## File Structure Explained

### `main.c`

- Core application logic
- CLI handling
- File operations
- Web server integration
- Program entry point

### `mongoose.c` / `mongoose.h`

- Embedded HTTP server
- Third-party dependency
- Kept isolated from core logic

### `localhostRun.sh`

- Convenience script to compile and run
- Enables web server by default

---

## Build Instructions

### Requirements

- GCC or compatible C compiler
- POSIX-compatible OS (Linux recommended)
- pthread support

---
### Build with web interface

```bash
gcc main.c mongoose.c -o file_manager -pthread -DENABLE_WEB_SERVER
```
---

## Build without web interface

```bash
gcc main.c mongoose.c -o file_manager
```

