#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef ENABLE_WEB_SERVER
#include <pthread.h>
#include "mongoose.h"
#include "api_handler.h"
#endif

// ANSI Color codes
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BG_BLUE "\033[44m"

// Terminal configuration
struct termios orig_termios;

// Enable raw mode for better input handling
void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Clear screen
void clearScreen() {
    printf("\033[2J\033[H");
}

// Draw fancy box
void drawBox(const char *title, const char *color) {
    printf("%s%s", color, BOLD);
    printf("╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                    ║\n");
    printf("║          %-58s║\n", title);
    printf("║                                                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    printf("%s", RESET);
}

// Loading animation
void showLoading(const char *message) {
    printf("%s%s%s", CYAN, BOLD, message);
    fflush(stdout);
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        usleep(300000);
    }
    printf(" ✓%s\n", RESET);
}

// Messages
void showSuccess(const char *message) {
    printf("%s%s✓ %s%s\n", GREEN, BOLD, message, RESET);
}

void showError(const char *message) {
    printf("%s%s✗ %s%s\n", RED, BOLD, message, RESET);
}

void showInfo(const char *message) {
    printf("%s%sℹ %s%s\n", BLUE, BOLD, message, RESET);
}

// Check if file exists
int fileExists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

// Confirm overwrite
int confirmOverwrite() {
    char choice;
    printf("%s%s⚠  File exists! Overwrite? (y/n): %s", YELLOW, BOLD, RESET);
    scanf(" %c", &choice);
    return (choice == 'y' || choice == 'Y');
}

// Enhanced editor with line editing support
void showEditorHeader(const char *filename, const char *status) {
    printf("%s%s", BG_BLUE, WHITE);
    printf("┌────────────────────────────────────────────────────────────────────┐\n");
    printf("│ 📝 EDITOR  │  File: %-48s │\n", filename);
    printf("│ Status: %-61s │\n", status);
    printf("└────────────────────────────────────────────────────────────────────┘\n");
    printf("%s", RESET);
}

void showLineNumber(int lineNum) {
    printf("%s%s%3d │ %s", DIM, CYAN, lineNum, RESET);
}

// Enhanced file editing with support for special keys
void getFileContent(FILE *fp, const char *filename) {
    clearScreen();
    showEditorHeader(filename, "EDITING");
    
    printf("\n%s%s", GREEN, BOLD);
    printf("  Commands:\n");
    printf("  • Type your code line by line\n");
    printf("  • Use arrow keys for navigation\n");
    printf("  • Type 'END' on a new line to save and exit\n");
    printf("  • Ctrl+D to save immediately\n");
    printf("%s\n", RESET);
    
    printf("%s%s────────────────────────────────────────────────────────────────────%s\n", 
           DIM, CYAN, RESET);
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    int lineCount = 1;
    char buffer[2000];
    
    while (1) {
        showLineNumber(lineCount);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        if (strcmp(buffer, "END\n") == 0 || strcmp(buffer, "END\r\n") == 0) {
            break;
        }
        
        fprintf(fp, "%s", buffer);
        fflush(fp);
        lineCount++;
    }
    
    printf("%s%s────────────────────────────────────────────────────────────────────%s\n", 
           DIM, CYAN, RESET);
    printf("\n");
    showLoading("Saving file");
    showSuccess("File saved successfully!");
    printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
    getchar();
}

// Display file with syntax awareness
void displayFile(const char *filename) {
    clearScreen();
    showEditorHeader(filename, "READ-ONLY");
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        showError("Cannot open file for reading");
        return;
    }
    
    printf("\n%s%s────────────────────────────────────────────────────────────────────%s\n", 
           DIM, CYAN, RESET);
    
    char buffer[2000];
    int lineNum = 1;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        showLineNumber(lineNum);
        printf("%s", buffer);
        lineNum++;
    }
    
    printf("%s%s────────────────────────────────────────────────────────────────────%s\n", 
           DIM, CYAN, RESET);
    
    fclose(fp);
    printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
    getchar();
    getchar();
}

// Welcome screen
void showWelcome() {
    clearScreen();
    printf("%s%s", CYAN, BOLD);
    printf("\n");
    printf("  ╔═══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                               ║\n");
    printf("  ║     🚀  ADVANCED FILE MANAGER & CODE EDITOR  🚀              ║\n");
    printf("  ║                                                               ║\n");
    printf("  ║                    Version 3.0                                ║\n");
    printf("  ║              Enhanced Terminal Support                        ║\n");
    printf("  ║                                                               ║\n");
    printf("  ╚═══════════════════════════════════════════════════════════════╝\n");
    printf("%s\n", RESET);
    
    showInfo("Features: Web UI, Terminal UI, Code Execution, Path Management");
    printf("\n%s%sPress Enter to start...%s", DIM, WHITE, RESET);
    getchar();
}

// Browse and select location
char* browseLocation() {
    static char selectedPath[1024] = ".";
    char currentPath[1024];
    getcwd(currentPath, sizeof(currentPath));
    
    while (1) {
        clearScreen();
        drawBox("LOCATION BROWSER", MAGENTA);
        
        printf("\n%s%sCurrent Path: %s%s\n\n", BOLD, CYAN, currentPath, RESET);
        
        DIR *dir = opendir(currentPath);
        if (!dir) {
            showError("Cannot open directory");
            return selectedPath;
        }
        
        struct dirent *entry;
        int count = 0;
        char dirs[100][256];
        
        printf("%s%sDirectories:%s\n", GREEN, BOLD, RESET);
        printf("  %s[0]%s ../ (Parent Directory)\n", CYAN, RESET);
        
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
                strncpy(dirs[count], entry->d_name, 255);
                printf("  %s[%d]%s 📁 %s\n", CYAN, count + 1, RESET, dirs[count]);
                count++;
            }
        }
        closedir(dir);
        
        printf("\n  %s[S]%s Select Current Directory\n", CYAN, RESET);
        printf("  %s[Q]%s Cancel\n", CYAN, RESET);
        
        printf("\n%s%sEnter choice: %s", YELLOW, BOLD, RESET);
        
        char input[10];
        if (scanf("%9s", input) != 1) continue;
        
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        if (input[0] == 'S' || input[0] == 's') {
            strncpy(selectedPath, currentPath, sizeof(selectedPath) - 1);
            showSuccess("Location selected!");
            sleep(1);
            return selectedPath;
        } else if (input[0] == 'Q' || input[0] == 'q') {
            return selectedPath;
        } else if (input[0] == '0') {
            // Go to parent directory
            if (chdir("..") == 0) {
                getcwd(currentPath, sizeof(currentPath));
            }
        } else {
            int choice = atoi(input);
            if (choice > 0 && choice <= count) {
                if (chdir(dirs[choice - 1]) == 0) {
                    getcwd(currentPath, sizeof(currentPath));
                }
            }
        }
    }
}

// Create new file with location selection
int createNewFile(char *filename, char files[][100], int fileCount) {
    clearScreen();
    drawBox("CREATE NEW FILE", MAGENTA);
    
    printf("\n%s%s📄 Enter filename: %s", CYAN, BOLD, RESET);
    if (scanf("%99s", filename) != 1) return 0;
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    printf("\n%s%sChoose location:%s\n", GREEN, BOLD, RESET);
    printf("  %s[1]%s Current directory (.)\n", CYAN, RESET);
    printf("  %s[2]%s Browse for location\n", CYAN, RESET);
    
    printf("\n%s%sChoice: %s", YELLOW, BOLD, RESET);
    int locChoice;
    if (scanf("%d", &locChoice) != 1) locChoice = 1;
    
    char *location = ".";
    if (locChoice == 2) {
        location = browseLocation();
    }
    
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", location, filename);
    
    // Check if file exists
    if (fileExists(fullPath)) {
        printf("\n");
        if (!confirmOverwrite()) {
            showError("Operation cancelled");
            printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
            getchar();
            getchar();
            return 0;
        }
    }
    
    FILE *fp = fopen(fullPath, "w");
    if (fp == NULL) {
        showError("Failed to create file");
        printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
        getchar();
        getchar();
        return 0;
    }
    
    printf("\n");
    showSuccess("File created successfully!");
    showLoading("Opening editor");
    
    getFileContent(fp, filename);
    fclose(fp);
    
    return 1;
}

// Execute code file
void executeCodeFile(const char *filename) {
    clearScreen();
    drawBox("CODE EXECUTION", GREEN);
    
    // Determine file type
    const char *ext = strrchr(filename, '.');
    if (!ext) {
        showError("Cannot determine file type");
        printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
        getchar();
        getchar();
        return;
    }
    ext++;
    
    printf("\n%s%sFile: %s%s\n", BOLD, CYAN, filename, RESET);
    printf("%s%sType: %s%s\n\n", BOLD, CYAN, ext, RESET);
    
    printf("%s%sExecution Options:%s\n", GREEN, BOLD, RESET);
    printf("  %s[1]%s Compile Only\n", CYAN, RESET);
    printf("  %s[2]%s Run Only\n", CYAN, RESET);
    printf("  %s[3]%s Compile and Run\n", CYAN, RESET);
    printf("  %s[4]%s Cancel\n", CYAN, RESET);
    
    printf("\n%s%sChoice: %s", YELLOW, BOLD, RESET);
    int choice;
    if (scanf("%d", &choice) != 1) return;
    
    if (choice == 4) return;
    
    char command[1024];
    printf("\n%s%s───────────────────────────────────────────────────%s\n", 
           DIM, CYAN, RESET);
    
    if (strcmp(ext, "c") == 0) {
        if (choice == 1) {
            snprintf(command, sizeof(command), "gcc -o %s.out %s", filename, filename);
        } else if (choice == 2) {
            snprintf(command, sizeof(command), "./%s.out", filename);
        } else if (choice == 3) {
            snprintf(command, sizeof(command), "gcc -o %s.out %s && ./%s.out", 
                    filename, filename, filename);
        }
    } else if (strcmp(ext, "cpp") == 0 || strcmp(ext, "cc") == 0) {
        if (choice == 1) {
            snprintf(command, sizeof(command), "g++ -o %s.out %s", filename, filename);
        } else if (choice == 2) {
            snprintf(command, sizeof(command), "./%s.out", filename);
        } else if (choice == 3) {
            snprintf(command, sizeof(command), "g++ -o %s.out %s && ./%s.out", 
                    filename, filename, filename);
        }
    } else if (strcmp(ext, "py") == 0) {
        snprintf(command, sizeof(command), "python3 %s", filename);
    } else if (strcmp(ext, "js") == 0) {
        snprintf(command, sizeof(command), "node %s", filename);
    } else if (strcmp(ext, "sh") == 0) {
        snprintf(command, sizeof(command), "bash %s", filename);
    } else if (strcmp(ext, "java") == 0) {
        if (choice == 1) {
            snprintf(command, sizeof(command), "javac %s", filename);
        } else if (choice == 2) {
            char className[256];
            strncpy(className, filename, strlen(filename) - 5);
            className[strlen(filename) - 5] = '\0';
            snprintf(command, sizeof(command), "java %s", className);
        } else if (choice == 3) {
            char className[256];
            strncpy(className, filename, strlen(filename) - 5);
            className[strlen(filename) - 5] = '\0';
            snprintf(command, sizeof(command), "javac %s && java %s", filename, className);
        }
    } else {
        showError("Unsupported file type for execution");
        printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
        getchar();
        getchar();
        return;
    }
    
    printf("%s%sExecuting: %s%s\n\n", BOLD, YELLOW, command, RESET);
    int result = system(command);
    
    printf("\n%s%s───────────────────────────────────────────────────%s\n", 
           DIM, CYAN, RESET);
    
    if (result == 0) {
        showSuccess("Execution completed successfully!");
    } else {
        showError("Execution failed!");
    }
    
    printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
    getchar();
    getchar();
}

// Main menu
void showMainMenu(char files[][100], int fileCount) {
    clearScreen();
    drawBox("MAIN MENU", BLUE);
    
    printf("\n%s%sYour Files:%s\n", GREEN, BOLD, RESET);
    for (int i = 0; i < fileCount; i++) {
        printf("  %s%d.%s %s📄 %s%s\n", CYAN, i + 1, RESET, YELLOW, files[i], RESET);
    }
    
    printf("\n%s%sActions:%s\n", MAGENTA, BOLD, RESET);
    printf("  %s[N]%s Create New File\n", CYAN, RESET);
    printf("  %s[E]%s Edit Existing File (1-%d)\n", CYAN, RESET, fileCount);
    printf("  %s[V]%s View File (1-%d)\n", CYAN, RESET, fileCount);
    printf("  %s[X]%s Execute Code File (1-%d)\n", CYAN, RESET, fileCount);
    printf("  %s[L]%s Browse Location\n", CYAN, RESET);
    printf("  %s[Q]%s Quit\n", CYAN, RESET);
    
    printf("\n%s%sChoose an option: %s", YELLOW, BOLD, RESET);
}

#ifdef ENABLE_WEB_SERVER
// Web server integration
static struct mg_mgr g_mgr;

// HTTP event handler - FIXED for new Mongoose API
static void http_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Parse query parameters
        char filename[256] = {0};
        char location[1024] = {0};
        
        mg_http_get_var(&hm->query, "file", filename, sizeof(filename));
        mg_http_get_var(&hm->query, "location", location, sizeof(location));
        mg_http_get_var(&hm->query, "path", location, sizeof(location));
        
        // Route requests - FIXED: Use mg_http_serve_opts
        if (mg_match(hm->uri, mg_str("/"), NULL)) {
            struct mg_http_serve_opts opts = {.root_dir = "."};
            mg_http_serve_file(c, hm, "index.html", &opts);
        } else if (mg_match(hm->uri, mg_str("/styles.css"), NULL)) {
            struct mg_http_serve_opts opts = {.root_dir = "."};
            mg_http_serve_file(c, hm, "styles.css", &opts);
        } else if (mg_match(hm->uri, mg_str("/style.css"), NULL)) {
            struct mg_http_serve_opts opts = {.root_dir = "."};
            mg_http_serve_file(c, hm, "style.css", &opts);
        } else if (mg_match(hm->uri, mg_str("/app.js"), NULL)) {
            struct mg_http_serve_opts opts = {.root_dir = "."};
            mg_http_serve_file(c, hm, "app.js", &opts);
        } else if (mg_match(hm->uri, mg_str("/api/files"), NULL)) {
            handle_list_files(c, location);
        } else if (mg_match(hm->uri, mg_str("/api/view"), NULL)) {
            handle_view_file(c, filename, location);
        } else if (mg_match(hm->uri, mg_str("/api/create"), NULL)) {
            char *fn = mg_json_get_str(hm->body, "$.filename");
            char *cnt = mg_json_get_str(hm->body, "$.content");
            char *loc = mg_json_get_str(hm->body, "$.location");
            handle_create_file(c, fn ? fn : "", cnt ? cnt : "", loc ? loc : "");
            free(fn); free(cnt); free(loc);
        } else if (mg_match(hm->uri, mg_str("/api/edit"), NULL)) {
            char *fn = mg_json_get_str(hm->body, "$.filename");
            char *cnt = mg_json_get_str(hm->body, "$.content");
            char *loc = mg_json_get_str(hm->body, "$.location");
            handle_edit_file(c, fn ? fn : "", cnt ? cnt : "", loc ? loc : "");
            free(fn); free(cnt); free(loc);
        } else if (mg_match(hm->uri, mg_str("/api/delete"), NULL)) {
            char *fn = mg_json_get_str(hm->body, "$.filename");
            char *loc = mg_json_get_str(hm->body, "$.location");
            handle_delete_file(c, fn ? fn : "", loc ? loc : "");
            free(fn); free(loc);
        } else if (mg_match(hm->uri, mg_str("/api/exists"), NULL)) {
            handle_file_exists(c, filename, location);
        } else if (mg_match(hm->uri, mg_str("/api/browse"), NULL)) {
            handle_browse_directories(c, location);
        } else if (mg_match(hm->uri, mg_str("/api/execute"), NULL)) {
            char *fn = mg_json_get_str(hm->body, "$.filename");
            char *act = mg_json_get_str(hm->body, "$.action");
            char *loc = mg_json_get_str(hm->body, "$.location");
            handle_execute_file(c, fn ? fn : "", act ? act : "run", loc ? loc : "");
            free(fn); free(act); free(loc);
        } else {
            mg_http_reply(c, 404, "", "Not found");
        }
    }
}

void *web_server_thread(void *arg) {
    mg_mgr_init(&g_mgr);
    mg_http_listen(&g_mgr, "http://0.0.0.0:8080", http_handler, NULL);
    
    printf("%s%s", GREEN, BOLD);
    printf("\n  ╔═══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                               ║\n");
    printf("  ║          🌐 Web Interface Started!                           ║\n");
    printf("  ║          Open: http://localhost:8080                         ║\n");
    printf("  ║                                                               ║\n");
    printf("  ╚═══════════════════════════════════════════════════════════════╝\n");
    printf("%s\n", RESET);
    
    while (1) {
        mg_mgr_poll(&g_mgr, 1000);
    }
    
    mg_mgr_free(&g_mgr);
    return NULL;
}
#endif

int main() {
    char files[100][100];
    int fileCount = 0;
    
#ifdef ENABLE_WEB_SERVER
    // Start web server
    pthread_t web_thread;
    pthread_create(&web_thread, NULL, web_server_thread, NULL);
    pthread_detach(web_thread);
    sleep(1);
    
    clearScreen();
    drawBox("INTERFACE SELECTION", CYAN);
    printf("\n%s%sThe Web Server is running on http://localhost:8080%s\n\n", 
           WHITE, BOLD, RESET);
    printf("How would you like to proceed?\n\n");
    printf("  %s[1]%s Continue in TERMINAL\n", GREEN, RESET);
    printf("  %s[2]%s Use WEB INTERFACE (Terminal will standby)\n", MAGENTA, RESET);
    printf("\n%s%sEnter choice: %s", YELLOW, BOLD, RESET);
    
    int choice;
    if (scanf("%d", &choice) != 1) choice = 1;
    
    if (choice == 2) {
        clearScreen();
        drawBox("WEB MODE ACTIVE", MAGENTA);
        showInfo("Terminal is on standby");
        showInfo("Use browser at http://localhost:8080");
        showInfo("Press Ctrl+C to exit");
        
        while(1) sleep(10);
    }
#endif
    
    showWelcome();
    
    clearScreen();
    drawBox("INITIAL SETUP", GREEN);
    printf("\n%s%sHow many files do you want to create? %s", CYAN, BOLD, RESET);
    int numFiles;
    if (scanf("%d", &numFiles) != 1) numFiles = 0;
    
    for (int i = 0; i < numFiles && fileCount < 100; i++) {
        if (createNewFile(files[fileCount], files, fileCount)) {
            fileCount++;
        }
    }
    
    // Main loop
    while (1) {
        showMainMenu(files, fileCount);
        
        char choice;
        if (scanf(" %c", &choice) != 1) continue;
        
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        if (choice == 'N' || choice == 'n') {
            if (fileCount < 100) {
                if (createNewFile(files[fileCount], files, fileCount)) {
                    fileCount++;
                }
            } else {
                showError("Maximum file limit reached!");
                sleep(1);
            }
        } else if (choice == 'E' || choice == 'e') {
            printf("Enter file number (1-%d): ", fileCount);
            int fileNum;
            if (scanf("%d", &fileNum) != 1) continue;
            if (fileNum >= 1 && fileNum <= fileCount) {
                FILE *fp = fopen(files[fileNum - 1], "a");
                if (fp) {
                    getFileContent(fp, files[fileNum - 1]);
                    fclose(fp);
                }
            }
        } else if (choice == 'V' || choice == 'v') {
            printf("Enter file number (1-%d): ", fileCount);
            int fileNum;
            if (scanf("%d", &fileNum) != 1) continue;
            if (fileNum >= 1 && fileNum <= fileCount) {
                displayFile(files[fileNum - 1]);
            }
        } else if (choice == 'X' || choice == 'x') {
            printf("Enter file number (1-%d): ", fileCount);
            int fileNum;
            if (scanf("%d", &fileNum) != 1) continue;
            if (fileNum >= 1 && fileNum <= fileCount) {
                executeCodeFile(files[fileNum - 1]);
            }
        } else if (choice == 'L' || choice == 'l') {
            browseLocation();
        } else if (choice == 'Q' || choice == 'q') {
            showLoading("Shutting down");
            showSuccess("Goodbye!");
            break;
        }
    }
    
    return 0;
}
