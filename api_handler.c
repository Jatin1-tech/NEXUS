#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "api_handler.h"
#include "mongoose.h"

#define MAX_OUTPUT_SIZE 8192

// Helper function to get file extension
const char* get_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

// Check if file exists
int file_exists(const char* filepath) {
    struct stat st;
    return stat(filepath, &st) == 0;
}

// List files in directory
void handle_list_files(struct mg_connection *c, const char *location) {
    char path[1024];
    if (location && strlen(location) > 0) {
        snprintf(path, sizeof(path), "%s", location);
    } else {
        strcpy(path, ".");
    }
    
    DIR *dir = opendir(path);
    if (!dir) {
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", 
                     "{\"error\": \"Cannot open directory\"}");
        return;
    }
    
    char response[8192] = "{\"files\": [";
    struct dirent *entry;
    int first = 1;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (!first) strcat(response, ", ");
            strcat(response, "\"");
            strcat(response, entry->d_name);
            strcat(response, "\"");
            first = 0;
        }
    }
    
    closedir(dir);
    strcat(response, "]}");
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
}

// View file contents
void handle_view_file(struct mg_connection *c, const char *filename, const char *location) {
    char filepath[1024];
    if (location && strlen(location) > 0) {
        snprintf(filepath, sizeof(filepath), "%s/%s", location, filename);
    } else {
        snprintf(filepath, sizeof(filepath), "%s", filename);
    }
    
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        mg_http_reply(c, 404, "Content-Type: application/json\r\n", 
                     "{\"error\": \"File not found\"}");
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(fp);
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", 
                     "{\"error\": \"Memory allocation failed\"}");
        return;
    }
    
    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);
    
    // Escape JSON special characters
    char *response = malloc(size * 2 + 100);
    strcpy(response, "{\"content\": \"");
    
    char *out = response + strlen(response);
    for (long i = 0; i < size; i++) {
        if (content[i] == '"') {
            *out++ = '\\';
            *out++ = '"';
        } else if (content[i] == '\\') {
            *out++ = '\\';
            *out++ = '\\';
        } else if (content[i] == '\n') {
            *out++ = '\\';
            *out++ = 'n';
        } else if (content[i] == '\r') {
            *out++ = '\\';
            *out++ = 'r';
        } else if (content[i] == '\t') {
            *out++ = '\\';
            *out++ = 't';
        } else {
            *out++ = content[i];
        }
    }
    *out = '\0';
    
    strcat(response, "\"}");
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
    
    free(content);
    free(response);
}

// Create new file
void handle_create_file(struct mg_connection *c, const char *filename, 
                       const char *content, const char *location) {
    char filepath[1024];
    if (location && strlen(location) > 0) {
        snprintf(filepath, sizeof(filepath), "%s/%s", location, filename);
    } else {
        snprintf(filepath, sizeof(filepath), "%s", filename);
    }
    
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", 
                     "{\"error\": \"Cannot create file\"}");
        return;
    }
    
    fprintf(fp, "%s", content);
    fclose(fp);
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                 "{\"success\": true, \"message\": \"File created successfully\"}");
}

// Edit file
void handle_edit_file(struct mg_connection *c, const char *filename, 
                     const char *content, const char *location) {
    handle_create_file(c, filename, content, location);
}

// Delete file
void handle_delete_file(struct mg_connection *c, const char *filename, const char *location) {
    char filepath[1024];
    if (location && strlen(location) > 0) {
        snprintf(filepath, sizeof(filepath), "%s/%s", location, filename);
    } else {
        snprintf(filepath, sizeof(filepath), "%s", filename);
    }
    
    if (remove(filepath) == 0) {
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                     "{\"success\": true, \"message\": \"File deleted\"}");
    } else {
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", 
                     "{\"error\": \"Cannot delete file\"}");
    }
}

// Check if file exists
void handle_file_exists(struct mg_connection *c, const char *filename, const char *location) {
    char filepath[1024];
    if (location && strlen(location) > 0) {
        snprintf(filepath, sizeof(filepath), "%s/%s", location, filename);
    } else {
        snprintf(filepath, sizeof(filepath), "%s", filename);
    }
    
    int exists = file_exists(filepath);
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                 "{\"exists\": %s}", exists ? "true" : "false");
}

// Browse directories
void handle_browse_directories(struct mg_connection *c, const char *path) {
    char dirpath[1024];
    if (path && strlen(path) > 0) {
        snprintf(dirpath, sizeof(dirpath), "%s", path);
    } else {
        strcpy(dirpath, ".");
    }
    
    DIR *dir = opendir(dirpath);
    if (!dir) {
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", 
                     "{\"error\": \"Cannot open directory\"}");
        return;
    }
    
    char response[8192];
    snprintf(response, sizeof(response), "{\"directories\": [");
    
    struct dirent *entry;
    int first = 1;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && 
            strcmp(entry->d_name, "..") != 0) {
            if (!first) strcat(response, ", ");
            strcat(response, "\"");
            strcat(response, entry->d_name);
            strcat(response, "\"");
            first = 0;
        }
    }
    
    closedir(dir);
    
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    
    strcat(response, "], \"currentPath\": \"");
    strcat(response, dirpath);
    strcat(response, "\"}");
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
}

// Execute code file with enhanced language support
void handle_execute_file(struct mg_connection *c, const char *filename, 
                        const char *action, const char *location) {
    char filepath[1024];
    if (location && strlen(location) > 0) {
        snprintf(filepath, sizeof(filepath), "%s/%s", location, filename);
    } else {
        snprintf(filepath, sizeof(filepath), "%s", filename);
    }
    
    const char *ext = get_extension(filename);
    char command[2048] = {0};
    char output_file[256];
    snprintf(output_file, sizeof(output_file), "/tmp/nexus_output_%d.txt", getpid());
    
    // Determine execution command based on file type
    if (strcmp(ext, "c") == 0) {
        char exe_name[256];
        snprintf(exe_name, sizeof(exe_name), "%s.out", filepath);
        
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "gcc -o %s %s 2>&1", exe_name, filepath);
        } else if (strcmp(action, "run") == 0) {
            snprintf(command, sizeof(command), "%s 2>&1", exe_name);
        } else { // both
            snprintf(command, sizeof(command), "gcc -o %s %s 2>&1 && %s 2>&1", 
                    exe_name, filepath, exe_name);
        }
    } 
    else if (strcmp(ext, "cpp") == 0 || strcmp(ext, "cc") == 0 || strcmp(ext, "cxx") == 0) {
        char exe_name[256];
        snprintf(exe_name, sizeof(exe_name), "%s.out", filepath);
        
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "g++ -std=c++17 -o %s %s 2>&1", exe_name, filepath);
        } else if (strcmp(action, "run") == 0) {
            snprintf(command, sizeof(command), "%s 2>&1", exe_name);
        } else {
            snprintf(command, sizeof(command), "g++ -std=c++17 -o %s %s 2>&1 && %s 2>&1", 
                    exe_name, filepath, exe_name);
        }
    }
    else if (strcmp(ext, "py") == 0) {
        snprintf(command, sizeof(command), "python3 %s 2>&1", filepath);
    }
    else if (strcmp(ext, "js") == 0) {
        snprintf(command, sizeof(command), "node %s 2>&1", filepath);
    }
    else if (strcmp(ext, "java") == 0) {
        char class_name[256];
        strncpy(class_name, filename, strlen(filename) - 5);
        class_name[strlen(filename) - 5] = '\0';
        
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "javac %s 2>&1", filepath);
        } else if (strcmp(action, "run") == 0) {
            snprintf(command, sizeof(command), "java %s 2>&1", class_name);
        } else {
            snprintf(command, sizeof(command), "javac %s 2>&1 && java %s 2>&1", 
                    filepath, class_name);
        }
    }
    else if (strcmp(ext, "sh") == 0 || strcmp(ext, "bash") == 0) {
        snprintf(command, sizeof(command), "bash %s 2>&1", filepath);
    }
    else if (strcmp(ext, "rb") == 0) {
        snprintf(command, sizeof(command), "ruby %s 2>&1", filepath);
    }
    else if (strcmp(ext, "go") == 0) {
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "go build %s 2>&1", filepath);
        } else if (strcmp(action, "run") == 0) {
            char exe_name[256];
            strncpy(exe_name, filename, strlen(filename) - 3);
            exe_name[strlen(filename) - 3] = '\0';
            snprintf(command, sizeof(command), "./%s 2>&1", exe_name);
        } else {
            snprintf(command, sizeof(command), "go run %s 2>&1", filepath);
        }
    }
    else if (strcmp(ext, "rs") == 0) {
        char exe_name[256];
        snprintf(exe_name, sizeof(exe_name), "%s.out", filepath);
        
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "rustc -o %s %s 2>&1", exe_name, filepath);
        } else if (strcmp(action, "run") == 0) {
            snprintf(command, sizeof(command), "%s 2>&1", exe_name);
        } else {
            snprintf(command, sizeof(command), "rustc -o %s %s 2>&1 && %s 2>&1", 
                    exe_name, filepath, exe_name);
        }
    }
    else if (strcmp(ext, "php") == 0) {
        snprintf(command, sizeof(command), "php %s 2>&1", filepath);
    }
    else if (strcmp(ext, "pl") == 0) {
        snprintf(command, sizeof(command), "perl %s 2>&1", filepath);
    }
    else if (strcmp(ext, "lua") == 0) {
        snprintf(command, sizeof(command), "lua %s 2>&1", filepath);
    }
    else if (strcmp(ext, "r") == 0 || strcmp(ext, "R") == 0) {
        snprintf(command, sizeof(command), "Rscript %s 2>&1", filepath);
    }
    else if (strcmp(ext, "swift") == 0) {
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "swiftc %s 2>&1", filepath);
        } else if (strcmp(action, "run") == 0) {
            char exe_name[256];
            strncpy(exe_name, filename, strlen(filename) - 6);
            exe_name[strlen(filename) - 6] = '\0';
            snprintf(command, sizeof(command), "./%s 2>&1", exe_name);
        } else {
            snprintf(command, sizeof(command), "swift %s 2>&1", filepath);
        }
    }
    else if (strcmp(ext, "kt") == 0) {
        char jar_name[256];
        snprintf(jar_name, sizeof(jar_name), "%s.jar", filepath);
        
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "kotlinc %s -include-runtime -d %s 2>&1", 
                    filepath, jar_name);
        } else if (strcmp(action, "run") == 0) {
            snprintf(command, sizeof(command), "java -jar %s 2>&1", jar_name);
        } else {
            snprintf(command, sizeof(command), "kotlinc %s -include-runtime -d %s 2>&1 && java -jar %s 2>&1", 
                    filepath, jar_name, jar_name);
        }
    }
    else if (strcmp(ext, "dart") == 0) {
        snprintf(command, sizeof(command), "dart %s 2>&1", filepath);
    }
    else if (strcmp(ext, "ts") == 0) {
        if (strcmp(action, "compile") == 0) {
            snprintf(command, sizeof(command), "tsc %s 2>&1", filepath);
        } else if (strcmp(action, "run") == 0) {
            char js_name[256];
            strncpy(js_name, filename, strlen(filename) - 3);
            js_name[strlen(filename) - 3] = '\0';
            strcat(js_name, ".js");
            snprintf(command, sizeof(command), "node %s 2>&1", js_name);
        } else {
            snprintf(command, sizeof(command), "ts-node %s 2>&1", filepath);
        }
    }
    else {
        mg_http_reply(c, 400, "Content-Type: application/json\r\n", 
                     "{\"error\": \"Unsupported file type: %s\"}", ext);
        return;
    }
    
    // Execute command and capture output
    strcat(command, " > ");
    strcat(command, output_file);
    strcat(command, " 2>&1");
    
    int result = system(command);
    
    // Read output
    FILE *output_fp = fopen(output_file, "r");
    char output[MAX_OUTPUT_SIZE] = {0};
    
    if (output_fp) {
        size_t bytes_read = fread(output, 1, MAX_OUTPUT_SIZE - 1, output_fp);
        output[bytes_read] = '\0';
        fclose(output_fp);
        remove(output_file);
    }
    
    // Escape output for JSON
    char escaped_output[MAX_OUTPUT_SIZE * 2] = {0};
    char *out = escaped_output;
    for (char *in = output; *in; in++) {
        if (*in == '"') {
            *out++ = '\\';
            *out++ = '"';
        } else if (*in == '\\') {
            *out++ = '\\';
            *out++ = '\\';
        } else if (*in == '\n') {
            *out++ = '\\';
            *out++ = 'n';
        } else if (*in == '\r') {
            *out++ = '\\';
            *out++ = 'r';
        } else if (*in == '\t') {
            *out++ = '\\';
            *out++ = 't';
        } else {
            *out++ = *in;
        }
    }
    *out = '\0';
    
    char response[MAX_OUTPUT_SIZE * 2 + 256];
    if (result == 0) {
        snprintf(response, sizeof(response), 
                "{\"success\": true, \"output\": \"%s\", \"exitCode\": %d}", 
                escaped_output, result);
    } else {
        snprintf(response, sizeof(response), 
                "{\"success\": false, \"output\": \"%s\", \"error\": \"Execution failed\", \"exitCode\": %d}", 
                escaped_output, result);
    }
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
}
