#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "mongoose.h"

// ============================================================================
// NEXUS File Manager - API Handler Header
// ============================================================================

/**
 * List all files in the specified directory
 * 
 * @param c Mongoose connection
 * @param location Directory path (default: ".")
 */
void handle_list_files(struct mg_connection *c, const char *location);

/**
 * View the contents of a file
 * 
 * @param c Mongoose connection
 * @param filename Name of the file to view
 * @param location Directory path
 */
void handle_view_file(struct mg_connection *c, const char *filename, const char *location);

/**
 * Create a new file with specified content
 * 
 * @param c Mongoose connection
 * @param filename Name of the new file
 * @param content Content to write to the file
 * @param location Directory path where file will be created
 */
void handle_create_file(struct mg_connection *c, const char *filename, 
                       const char *content, const char *location);

/**
 * Edit an existing file (overwrites content)
 * 
 * @param c Mongoose connection
 * @param filename Name of the file to edit
 * @param content New content for the file
 * @param location Directory path
 */
void handle_edit_file(struct mg_connection *c, const char *filename, 
                     const char *content, const char *location);

/**
 * Delete a file
 * 
 * @param c Mongoose connection
 * @param filename Name of the file to delete
 * @param location Directory path
 */
void handle_delete_file(struct mg_connection *c, const char *filename, const char *location);

/**
 * Check if a file exists
 * 
 * @param c Mongoose connection
 * @param filename Name of the file to check
 * @param location Directory path
 */
void handle_file_exists(struct mg_connection *c, const char *filename, const char *location);

/**
 * Browse directories at the specified path
 * 
 * @param c Mongoose connection
 * @param path Directory path to browse
 */
void handle_browse_directories(struct mg_connection *c, const char *path);

/**
 * Execute/compile a code file
 * Supports 25+ programming languages including:
 * - C, C++, Java, Python, JavaScript, TypeScript
 * - Go, Rust, Swift, Kotlin, Dart
 * - Ruby, PHP, Perl, Lua, R
 * - Shell scripts (Bash, Zsh, Fish)
 * 
 * @param c Mongoose connection
 * @param filename Name of the file to execute
 * @param action Execution action: "compile", "run", or "both"
 * @param location Directory path
 */
void handle_execute_file(struct mg_connection *c, const char *filename, 
                        const char *action, const char *location);

#endif // API_HANDLER_H
