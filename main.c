#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ============================================================================
// NEW ADDITION: Mongoose HTTP Server Integration
// ============================================================================
#ifdef ENABLE_WEB_SERVER
#include <pthread.h>
#endif

#define MG_ENABLE_PACKED_FS 1
#include "mongoose.h"

// Global file list for web API
static char g_files[100][100];
static int g_fileCount = 0;
static struct mg_mgr g_mgr;

// ============================================================================
// END NEW ADDITION
// ============================================================================

// ANSI Color codes for beautiful UI
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"

// Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// Background colors
#define BG_BLUE "\033[44m"
#define BG_GREEN "\033[42m"
#define BG_CYAN "\033[46m"

// Clear screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Draw a fancy box
void drawBox(const char *title, const char *color) {
    printf("%s%s", color, BOLD);
    printf("╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                    ║\n");
    printf("║          %-58s║\n", title);
    printf("║                                                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n");
    printf("%s", RESET);
}

// Animated loading
void showLoading(const char *message) {
    printf("%s%s%s", CYAN, BOLD, message);
    fflush(stdout);
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        #ifdef _WIN32
            Sleep(300);
        #else
            usleep(300000);
        #endif
    }
    printf(" ✓%s\n", RESET);
}

// Show success message
void showSuccess(const char *message) {
    printf("%s%s✓ %s%s\n", GREEN, BOLD, message, RESET);
}

// Show error message
void showError(const char *message) {
    printf("%s%s✗ %s%s\n", RED, BOLD, message, RESET);
}

// Show info message
void showInfo(const char *message) {
    printf("%s%sℹ %s%s\n", BLUE, BOLD, message, RESET);
}

// Function to check if file exists
int fileExists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fclose(fp);
        return 1;
    }
    return 0;
}

// Function to ask user for overwrite confirmation
int confirmOverwrite() {
    char choice;
    printf("%s%s⚠  File exists! Overwrite? (y/n): %s", YELLOW, BOLD, RESET);
    scanf(" %c", &choice);
    return (choice == 'y' || choice == 'Y');
}

// Editor-like header
void showEditorHeader(const char *filename, const char *status) {
    printf("%s%s", BG_BLUE, WHITE);
    printf("┌──────────────────────────────────────────────────────────────────────┐\n");
    printf("│ 📝 EDITOR  │  File: %-48s │\n", filename);
    printf("│ Status: %-61s │\n", status);
    printf("└──────────────────────────────────────────────────────────────────────┘\n");
    printf("%s", RESET);
}

// Line number display
void showLineNumber(int lineNum) {
    printf("%s%s%3d │ %s", DIM, CYAN, lineNum, RESET);
}

// Get file content with VS Code-like interface
void getFileContent(FILE *fp, const char *filename) {
    char buffer[2000];
    
    clearScreen();
    showEditorHeader(filename, "EDITING");
    
    printf("\n%s%s", GREEN, BOLD);
    printf("  Commands:\n");
    printf("  • Type your code line by line\n");
    printf("  • Type 'END' on a new line to save and exit\n");
    printf("  • Press Ctrl+D (Linux/Mac) or Ctrl+Z+Enter (Windows) to save\n");
    printf("%s\n", RESET);
    
    printf("%s%s─────────────────────────────────────────────────────────────────────────%s\n", DIM, CYAN, RESET);
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    int lineCount = 1;
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
    
    printf("%s%s─────────────────────────────────────────────────────────────────────────%s\n", DIM, CYAN, RESET);
    printf("\n");
    showLoading("Saving file");
    showSuccess("File saved successfully!");
    printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
    getchar();
}

// Display file content with syntax highlighting feel
void displayFile(const char *filename) {
    clearScreen();
    showEditorHeader(filename, "READ-ONLY");
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        showError("Cannot open file for reading");
        return;
    }
    
    printf("\n%s%s─────────────────────────────────────────────────────────────────────────%s\n", DIM, CYAN, RESET);
    
    char buffer[2000];
    int lineNum = 1;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        showLineNumber(lineNum);
        printf("%s", buffer);
        lineNum++;
    }
    
    printf("%s%s─────────────────────────────────────────────────────────────────────────%s\n", DIM, CYAN, RESET);
    
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
    printf("  ║                    Version 2.0                                ║\n");
    printf("  ║                                                               ║\n");
    printf("  ╚═══════════════════════════════════════════════════════════════╝\n");
    printf("%s\n", RESET);
    
    showInfo("Create and edit multiple files with ease!");
    printf("\n%s%sPress Enter to start...%s", DIM, WHITE, RESET);
    getchar();
}

// Create a new file
int createNewFile(char *filename) {
    clearScreen();
    drawBox("CREATE NEW FILE", MAGENTA);
    
    printf("\n%s%s📄 Enter filename: %s", CYAN, BOLD, RESET);
    scanf("%99s", filename);
    
    // Check if file exists
    if (fileExists(filename)) {
        printf("\n");
        if (!confirmOverwrite()) {
            showError("Operation cancelled");
            printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
            getchar();
            getchar();
            return 0;
        }
    }
    
    FILE *fp = fopen(filename, "w");
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

// Edit existing file
void editExistingFile(const char *filename) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        showError("Failed to open file for editing");
        printf("\n%s%sPress Enter to continue...%s", DIM, WHITE, RESET);
        getchar();
        getchar();
        return;
    }
    
    showLoading("Opening editor");
    getFileContent(fp, filename);
    fclose(fp);
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
    printf("  %s[Q]%s Quit\n", CYAN, RESET);
    
    printf("\n%s%sChoose an option: %s", YELLOW, BOLD, RESET);
}

// ============================================================================
// NEW ADDITION: Web Server Implementation
// ============================================================================

// Embedded HTML/CSS/JS for Android-style Material Design UI
static const char *s_web_root = 
"<!DOCTYPE html>"
"<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>File Manager</title><style>"
"*{margin:0;padding:0;box-sizing:border-box}body{font-family:'Roboto',sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px}"
".container{max-width:800px;margin:0 auto;background:#fff;border-radius:16px;box-shadow:0 10px 40px rgba(0,0,0,0.2);overflow:hidden}"
".header{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#fff;padding:24px;text-align:center}"
".header h1{font-size:28px;font-weight:500;margin-bottom:8px}"
".header p{opacity:0.9;font-size:14px}"
".content{padding:24px}"
".btn{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#fff;border:none;padding:14px 28px;border-radius:8px;font-size:16px;cursor:pointer;width:100%;margin-bottom:16px;box-shadow:0 4px 12px rgba(102,126,234,0.4);transition:all 0.3s}"
".btn:hover{transform:translateY(-2px);box-shadow:0 6px 16px rgba(102,126,234,0.6)}"
".btn:active{transform:translateY(0)}"
".file-list{margin-top:24px}"
".file-item{background:#f5f5f5;padding:16px;margin-bottom:12px;border-radius:8px;display:flex;justify-content:space-between;align-items:center;transition:all 0.3s}"
".file-item:hover{background:#e8e8e8;transform:translateX(4px)}"
".file-name{font-weight:500;color:#333;font-size:16px}"
".file-actions{display:flex;gap:8px}"
".btn-small{padding:8px 16px;font-size:14px;border-radius:6px;background:#667eea;color:#fff;border:none;cursor:pointer;transition:all 0.3s}"
".btn-small:hover{background:#5568d3}"
".modal{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.6);z-index:1000;justify-content:center;align-items:center}"
".modal-content{background:#fff;padding:32px;border-radius:16px;max-width:600px;width:90%;max-height:80vh;overflow-y:auto;box-shadow:0 20px 60px rgba(0,0,0,0.3)}"
".modal h2{margin-bottom:20px;color:#333}"
"input,textarea{width:100%;padding:14px;margin-bottom:16px;border:2px solid #e0e0e0;border-radius:8px;font-size:16px;font-family:inherit;transition:border 0.3s}"
"input:focus,textarea:focus{outline:none;border-color:#667eea}"
"textarea{min-height:300px;font-family:'Courier New',monospace;resize:vertical}"
".btn-group{display:flex;gap:12px}"
".btn-cancel{background:#e0e0e0;color:#333}"
".btn-cancel:hover{background:#d0d0d0}"
".empty-state{text-align:center;padding:48px;color:#999}"
".empty-state svg{width:120px;height:120px;margin-bottom:24px;opacity:0.3}"
"@keyframes fadeIn{from{opacity:0;transform:translateY(20px)}to{opacity:1;transform:translateY(0)}}"
".file-item{animation:fadeIn 0.3s ease}"
"</style></head><body>"
"<div class='container'>"
"<div class='header'><h1>📁 File Manager</h1><p>Manage your files with ease</p></div>"
"<div class='content'>"
"<button class='btn' onclick='showCreateModal()'>➕ Create New File</button>"
"<div class='file-list' id='fileList'></div>"
"</div></div>"
"<div class='modal' id='createModal'>"
"<div class='modal-content'><h2>Create New File</h2>"
"<input type='text' id='newFileName' placeholder='Enter filename...'>"
"<textarea id='newFileContent' placeholder='Enter file content...'></textarea>"
"<div class='btn-group'><button class='btn' onclick='createFile()'>Create</button>"
"<button class='btn btn-cancel' onclick='closeModal(\"createModal\")'>Cancel</button></div></div></div>"
"<div class='modal' id='editModal'>"
"<div class='modal-content'><h2>Edit File: <span id='editFileName'></span></h2>"
"<textarea id='editFileContent'></textarea>"
"<div class='btn-group'><button class='btn' onclick='saveFile()'>Save</button>"
"<button class='btn btn-cancel' onclick='closeModal(\"editModal\")'>Cancel</button></div></div></div>"
"<div class='modal' id='viewModal'>"
"<div class='modal-content'><h2>View File: <span id='viewFileName'></span></h2>"
"<textarea id='viewFileContent' readonly></textarea>"
"<button class='btn btn-cancel' onclick='closeModal(\"viewModal\")'>Close</button></div></div>"
"<script>"
"let currentEditFile='';"
"async function loadFiles(){"
"const res=await fetch('/api/files');"
"const data=await res.json();"
"const list=document.getElementById('fileList');"
"if(data.files.length===0){list.innerHTML='<div class=\"empty-state\"><svg viewBox=\"0 0 24 24\" fill=\"currentColor\"><path d=\"M19 3H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h14c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm0 16H5V5h14v14z\"/></svg><p>No files yet. Create your first file!</p></div>';return}"
"list.innerHTML=data.files.map(f=>`<div class='file-item'><span class='file-name'>📄 ${f}</span><div class='file-actions'><button class='btn-small' onclick='viewFile(\"${f}\")'>View</button><button class='btn-small' onclick='editFile(\"${f}\")'>Edit</button></div></div>`).join('');}"
"function showCreateModal(){document.getElementById('createModal').style.display='flex';document.getElementById('newFileName').value='';document.getElementById('newFileContent').value='';}"
"function closeModal(id){document.getElementById(id).style.display='none';}"
"async function createFile(){"
"const name=document.getElementById('newFileName').value;"
"const content=document.getElementById('newFileContent').value;"
"if(!name){alert('Please enter a filename');return}"
"await fetch('/api/create',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({filename:name,content:content})});"
"closeModal('createModal');loadFiles();}"
"async function editFile(name){"
"currentEditFile=name;"
"const res=await fetch(`/api/view?file=${encodeURIComponent(name)}`);"
"const data=await res.json();"
"document.getElementById('editFileName').textContent=name;"
"document.getElementById('editFileContent').value=data.content;"
"document.getElementById('editModal').style.display='flex';}"
"async function saveFile(){"
"const content=document.getElementById('editFileContent').value;"
"await fetch('/api/edit',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({filename:currentEditFile,content:content})});"
"closeModal('editModal');loadFiles();}"
"async function viewFile(name){"
"const res=await fetch(`/api/view?file=${encodeURIComponent(name)}`);"
"const data=await res.json();"
"document.getElementById('viewFileName').textContent=name;"
"document.getElementById('viewFileContent').value=data.content;"
"document.getElementById('viewModal').style.display='flex';}"
"document.querySelectorAll('.modal').forEach(m=>m.onclick=e=>{if(e.target===m)m.style.display='none'});"
"loadFiles();"
"</script></body></html>";

// API handler for listing files
static void handle_list_files(struct mg_connection *c) {
    char json[4096] = "{\"files\":[";
    for (int i = 0; i < g_fileCount; i++) {
        char buf[120];
        snprintf(buf, sizeof(buf), "%s\"%s\"", i > 0 ? "," : "", g_files[i]);
        strcat(json, buf);
    }
    strcat(json, "]}");
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
}

// API handler for viewing file content
static void handle_view_file(struct mg_connection *c, struct mg_http_message *hm) {
    char filename[256] = {0};
    mg_http_get_var(&hm->query, "file", filename, sizeof(filename));
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        mg_http_reply(c, 404, "", "{\"error\":\"File not found\"}");
        return;
    }
    
    char content[10000] = {0};
    fread(content, 1, sizeof(content) - 1, fp);
    fclose(fp);
    
    // Escape quotes in content for JSON
    char escaped[20000] = {0};
    int j = 0;
    for (int i = 0; content[i] && j < sizeof(escaped) - 2; i++) {
        if (content[i] == '"' || content[i] == '\\') {
            escaped[j++] = '\\';
        }
        if (content[i] == '\n') {
            escaped[j++] = '\\';
            escaped[j++] = 'n';
        } else if (content[i] == '\r') {
            escaped[j++] = '\\';
            escaped[j++] = 'r';
        } else if (content[i] == '\t') {
            escaped[j++] = '\\';
            escaped[j++] = 't';
        } else {
            escaped[j++] = content[i];
        }
    }
    
    char json[25000];
    snprintf(json, sizeof(json), "{\"content\":\"%s\"}", escaped);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
}

// API handler for creating files
static void handle_create_file(struct mg_connection *c, struct mg_http_message *hm) {
    char filename[256] = {0};
    char content[10000] = {0};
    
    // Extract filename using new Mongoose API
    char *fn = mg_json_get_str(hm->body, "$.filename");
    if (fn != NULL) {
        strncpy(filename, fn, sizeof(filename) - 1);
        free(fn);
    }
    
    // Extract content using new Mongoose API
    char *cnt = mg_json_get_str(hm->body, "$.content");
    if (cnt != NULL) {
        strncpy(content, cnt, sizeof(content) - 1);
        free(cnt);
    }
    
    if (strlen(filename) == 0) {
        mg_http_reply(c, 400, "", "{\"error\":\"Filename is required\"}");
        return;
    }
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        mg_http_reply(c, 500, "", "{\"error\":\"Failed to create file\"}");
        return;
    }
    
    fprintf(fp, "%s", content);
    fclose(fp);
    
    // Add to global file list
    if (g_fileCount < 100) {
        strncpy(g_files[g_fileCount], filename, 99);
        g_fileCount++;
    }
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"success\":true}");
}

// API handler for editing files
static void handle_edit_file(struct mg_connection *c, struct mg_http_message *hm) {
    char filename[256] = {0};
    char content[10000] = {0};
    
    // Extract filename using new Mongoose API
    char *fn = mg_json_get_str(hm->body, "$.filename");
    if (fn != NULL) {
        strncpy(filename, fn, sizeof(filename) - 1);
        free(fn);
    }
    
    // Extract content using new Mongoose API
    char *cnt = mg_json_get_str(hm->body, "$.content");
    if (cnt != NULL) {
        strncpy(content, cnt, sizeof(content) - 1);
        free(cnt);
    }
    
    if (strlen(filename) == 0) {
        mg_http_reply(c, 400, "", "{\"error\":\"Filename is required\"}");
        return;
    }
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        mg_http_reply(c, 500, "", "{\"error\":\"Failed to edit file\"}");
        return;
    }
    
    fprintf(fp, "%s", content);
    fclose(fp);
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"success\":true}");
}

// HTTP event handler
static void http_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        if (mg_match(hm->uri, mg_str("/"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", s_web_root);
        } else if (mg_match(hm->uri, mg_str("/api/files"), NULL)) {
            handle_list_files(c);
        } else if (mg_match(hm->uri, mg_str("/api/view"), NULL)) {
            handle_view_file(c, hm);
        } else if (mg_match(hm->uri, mg_str("/api/create"), NULL)) {
            handle_create_file(c, hm);
        } else if (mg_match(hm->uri, mg_str("/api/edit"), NULL)) {
            handle_edit_file(c, hm);
        } else {
            mg_http_reply(c, 404, "", "Not found");
        }
    }
}

#ifdef ENABLE_WEB_SERVER
// Start web server in background thread
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

// ============================================================================
// END NEW ADDITION
// ============================================================================
// Function to ask user for preferred environment
int chooseEnvironment() {
    int choice;
    clearScreen();
    drawBox("SELECT INTERFACE", CYAN);
    printf("\n%s%sHow would you like to manage your files?%s\n", WHITE, BOLD, RESET);
    printf("  %s[1]%s Use Terminal Only (Default)\n", CYAN, RESET);
    printf("  %s[2]%s Start Web Interface (Localhost:8080)\n", CYAN, RESET);
    printf("  %s[3]%s Start Both\n", CYAN, RESET);
    
    printf("\n%s%sEnter choice (1-3): %s", YELLOW, BOLD, RESET);
    if (scanf("%d", &choice) != 1) return 1;
    return choice;
}

int main() {
    char files[100][100];
    int fileCount = 0;
    int choice;

    // Start the web server automatically if compiled with the flag
    #ifdef ENABLE_WEB_SERVER
    pthread_t web_thread;
    pthread_create(&web_thread, NULL, web_server_thread, NULL);
    pthread_detach(web_thread);
    #endif

    clearScreen();
    drawBox("ENVIRONMENT SELECTOR", CYAN);
    printf("\n%s%sThe Web Server is starting on http://localhost:8080%s\n", WHITE, BOLD, RESET);
    printf("How would you like to proceed?\n\n");
    printf("  %s[1]%s Continue in TERMINAL (Standard Mode)\n", GREEN, RESET);
    printf("  %s[2]%s Switch to WEB INTERFACE (Terminal will wait)\n", MAGENTA, RESET);
    printf("\n%s%sEnter choice: %s", YELLOW, BOLD, RESET);
    
    scanf("%d", &choice);

    if (choice == 2) {
        clearScreen();
        drawBox("WEB MODE ACTIVE", MAGENTA);
        showInfo("Terminal is now on standby.");
        showInfo("Use your browser at http://localhost:8080 to manage files.");
        showInfo("Press Ctrl+C in this terminal to exit entirely.");
        system("chmod +x localhostRun.sh");
        system("./localhostRun.sh");        
        // Keep the main thread alive so the web server thread doesn't die
        while(1) {
            #ifdef _WIN32
                Sleep(1000);
            #else
                sleep(1);
            #endif
        }
    }

    // Otherwise, proceed to terminal setup
    showWelcome();
/*int main() {
    char files[100][100];
    int fileCount = 0;
    
    int envChoice = chooseEnvironment();

    // ============================================================================
    // MODIFIED: Conditional Start of web server
    // ============================================================================
    #ifdef ENABLE_WEB_SERVER
    if (envChoice == 2 || envChoice == 3) {
        pthread_t web_thread;
        pthread_create(&web_thread, NULL, web_server_thread, NULL);
        pthread_detach(web_thread);
        sleep(1); // Give server time to start
        
        if (envChoice == 2) {
            showInfo("Web server is running. Terminal management is disabled.");
            showInfo("Press Ctrl+C to shut down the server.");
            while(1) sleep(10); // Keep main thread alive for the web server
        }
    }
    #endif
    // ============================================================================

    // If choice was 1 or 3, continue to Terminal UI
    showWelcome();*/
    
    // Ask how many files to create initially
    clearScreen();
    drawBox("INITIAL SETUP", GREEN);
    printf("\n%s%sHow many files do you want to create? %s", CYAN, BOLD, RESET);
    int numFiles;
    if (scanf("%d", &numFiles) != 1) numFiles = 0;
    

/*int main() {
    char files[100][100];
    int fileCount = 0;
    
    // ============================================================================
    // NEW ADDITION: Start web server in background
    // ============================================================================
    #ifdef ENABLE_WEB_SERVER
    pthread_t web_thread;
    pthread_create(&web_thread, NULL, web_server_thread, NULL);
    pthread_detach(web_thread);
    sleep(1); // Give server time to start
    #endif
    // ============================================================================
    // END NEW ADDITION
    // ============================================================================
    
    showWelcome();
    
    // Ask how many files to create initially
    clearScreen();
    drawBox("INITIAL SETUP", GREEN);
    printf("\n%s%sHow many files do you want to create? %s", CYAN, BOLD, RESET);
    int numFiles;
    scanf("%d", &numFiles);*/
    
    // Create initial files
    for (int i = 0; i < numFiles; i++) {
        clearScreen();
        printf("%s%s", MAGENTA, BOLD);
        printf("\n  Creating file %d of %d\n\n", i + 1, numFiles);
        printf("%s", RESET);
        
        if (createNewFile(files[fileCount])) {
            // ============================================================================
            // NEW ADDITION: Sync with global file list for web UI
            // ============================================================================
            strncpy(g_files[g_fileCount], files[fileCount], 99);
            g_fileCount++;
            // ============================================================================
            // END NEW ADDITION
            // ============================================================================
            fileCount++;
        }
    }
    
    // Main loop
    while (1) {
        showMainMenu(files, fileCount);
        
        char choice;
        scanf(" %c", &choice);
        
        if (choice == 'N' || choice == 'n') {
            if (fileCount < 100) {
                if (createNewFile(files[fileCount])) {
                    // ============================================================================
                    // NEW ADDITION: Sync with global file list
                    // ============================================================================
                    strncpy(g_files[g_fileCount], files[fileCount], 99);
                    g_fileCount++;
                    // ============================================================================
                    // END NEW ADDITION
                    // ============================================================================
                    fileCount++;
                }
            } else {
                showError("Maximum file limit reached!");
            }
        } else if (choice == 'E' || choice == 'e') {
            printf("Enter file number (1-%d): ", fileCount);
            int fileNum;
            scanf("%d", &fileNum);
            if (fileNum >= 1 && fileNum <= fileCount) {
                editExistingFile(files[fileNum - 1]);
            } else {
                showError("Invalid file number!");
                getchar();
                getchar();
            }
        } else if (choice == 'V' || choice == 'v') {
            printf("Enter file number (1-%d): ", fileCount);
            int fileNum;
            scanf("%d", &fileNum);
            if (fileNum >= 1 && fileNum <= fileCount) {
                displayFile(files[fileNum - 1]);
            } else {
                showError("Invalid file number!");
                // Pause to let user see error
                while (getchar() != '\n'); 
                getchar();
            }
        } else if (choice == 'Q' || choice == 'q') {
            showLoading("Shutting down");
            showSuccess("Goodbye!");
            break; // Exit the while(1) loop
        } else {
            showError("Invalid choice! Please try again.");
            // Clear input buffer
            while (getchar() != '\n');
        }
    }

    return 0;
}
