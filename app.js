// ============================================================================
// Global State Management
// ============================================================================
const state = {
    files: [],
    currentLocation: '.',
    currentEditFile: null,
    currentExecuteFile: null,
    viewMode: 'grid'
};

// ============================================================================
// Initialization
// ============================================================================
document.addEventListener('DOMContentLoaded', () => {
    initializeApp();
    setupEventListeners();
    loadFiles();
});

function initializeApp() {
    // Note: NOT using localStorage as per requirements
    // Location will reset on page refresh
    state.currentLocation = '.';
}

// ============================================================================
// Event Listeners Setup
// ============================================================================
function setupEventListeners() {
    // Create File Modal
    document.getElementById('createFileBtn').addEventListener('click', openCreateModal);
    document.getElementById('confirmCreate').addEventListener('click', createFile);
    document.getElementById('cancelCreate').addEventListener('click', () => closeModal('createModal'));
    
    // Edit File Modal
    document.getElementById('saveEdit').addEventListener('click', saveFile);
    document.getElementById('cancelEdit').addEventListener('click', () => closeModal('editModal'));
    
    // View File Modal
    document.getElementById('closeView').addEventListener('click', () => closeModal('viewModal'));
    
    // Execute Modal
    document.getElementById('compileBtn').addEventListener('click', () => executeFile('compile'));
    document.getElementById('runBtn').addEventListener('click', () => executeFile('run'));
    document.getElementById('compileRunBtn').addEventListener('click', () => executeFile('both'));
    document.getElementById('closeExecute').addEventListener('click', () => closeModal('executeModal'));
    
    // Location Modal
    document.getElementById('browseLocation').addEventListener('click', openLocationBrowser);
    document.getElementById('locationBtn').addEventListener('click', openLocationBrowser);
    document.getElementById('selectLocation').addEventListener('click', selectLocation);
    document.getElementById('cancelLocation').addEventListener('click', () => closeModal('locationModal'));
    
    // Overwrite Modal
    document.getElementById('confirmOverwrite').addEventListener('click', confirmOverwrite);
    document.getElementById('cancelOverwrite').addEventListener('click', () => closeModal('overwriteModal'));
    
    // Search
    document.getElementById('searchInput').addEventListener('input', handleSearch);
    
    // View Toggle
    document.querySelectorAll('.view-btn').forEach(btn => {
        btn.addEventListener('click', (e) => toggleView(e.target.closest('.view-btn').dataset.view));
    });
    
    // Modal Close Buttons
    document.querySelectorAll('.modal-close').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const modal = e.target.closest('.modal');
            closeModal(modal.id);
        });
    });
    
    // Modal Overlay Click
    document.querySelectorAll('.modal-overlay').forEach(overlay => {
        overlay.addEventListener('click', (e) => {
            const modal = e.target.closest('.modal');
            closeModal(modal.id);
        });
    });
    
    // Navigation
    document.querySelectorAll('.nav-item').forEach(item => {
        item.addEventListener('click', (e) => {
            e.preventDefault();
            handleNavigation(item.dataset.section);
        });
    });
}

// ============================================================================
// API Communication
// ============================================================================
async function apiCall(endpoint, method = 'GET', data = null) {
    const options = {
        method,
        headers: {
            'Content-Type': 'application/json'
        }
    };
    
    if (data) {
        options.body = JSON.stringify(data);
    }
    
    try {
        const response = await fetch(endpoint, options);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return await response.json();
    } catch (error) {
        console.error('API Error:', error);
        showNotification('Error communicating with server', 'error');
        throw error;
    }
}

// ============================================================================
// File Loading and Display
// ============================================================================
async function loadFiles() {
    try {
        const data = await apiCall('/api/files');
        state.files = data.files.map(name => ({
            name,
            extension: getFileExtension(name),
            icon: getFileIcon(name),
            isCode: isCodeFile(name)
        }));
        
        displayFiles(state.files);
        updateStats();
    } catch (error) {
        console.error('Failed to load files:', error);
    }
}

function displayFiles(files) {
    const fileGrid = document.getElementById('fileGrid');
    
    if (files.length === 0) {
        fileGrid.innerHTML = `
            <div class="empty-state">
                <svg viewBox="0 0 24 24" fill="currentColor">
                    <path d="M9 2a1 1 0 000 2h2a1 1 0 100-2H9z"/>
                    <path fill-rule="evenodd" d="M4 5a2 2 0 012-2 3 3 0 003 3h2a3 3 0 003-3 2 2 0 012 2v11a2 2 0 01-2 2H6a2 2 0 01-2-2V5z"/>
                </svg>
                <h3>No files yet</h3>
                <p>Create your first file to get started with NEXUS!</p>
            </div>
        `;
        return;
    }
    
    fileGrid.innerHTML = files.map(file => `
        <div class="file-card" data-filename="${file.name}">
            <div class="file-header">
                <div class="file-icon">${file.icon}</div>
                <div class="file-info">
                    <div class="file-name">${file.name}</div>
                    <div class="file-meta">${file.extension.toUpperCase()} File</div>
                </div>
            </div>
            <div class="file-actions">
                <button class="file-action-btn" onclick="viewFile('${file.name}')">
                    👁️ View
                </button>
                <button class="file-action-btn" onclick="editFile('${file.name}')">
                    ✏️ Edit
                </button>
                ${file.isCode ? `
                    <button class="file-action-btn" onclick="openExecuteModal('${file.name}')">
                        ▶️ Run
                    </button>
                ` : ''}
                <button class="file-action-btn danger" onclick="deleteFile('${file.name}')">
                    🗑️
                </button>
            </div>
        </div>
    `).join('');
}

function updateStats() {
    const totalFiles = state.files.length;
    const codeFiles = state.files.filter(f => f.isCode).length;
    const recentFiles = Math.min(totalFiles, 5);
    
    document.getElementById('totalFiles').textContent = totalFiles;
    document.getElementById('codeFiles').textContent = codeFiles;
    document.getElementById('recentFiles').textContent = recentFiles;
}

// ============================================================================
// File Operations
// ============================================================================
async function openCreateModal() {
    document.getElementById('fileLocation').value = state.currentLocation;
    openModal('createModal');
}

async function createFile() {
    const filename = document.getElementById('newFileName').value.trim();
    const content = document.getElementById('newFileContent').value;
    const location = document.getElementById('fileLocation').value;
    
    if (!filename) {
        showNotification('Please enter a filename', 'warning');
        return;
    }
    
    // Check if file exists
    const exists = await checkFileExists(filename, location);
    if (exists) {
        // Show overwrite modal
        openModal('overwriteModal');
        return;
    }
    
    await performCreateFile(filename, content, location);
}

async function performCreateFile(filename, content, location) {
    try {
        await apiCall('/api/create', 'POST', {
            filename,
            content,
            location
        });
        
        showNotification('File created successfully!', 'success');
        closeModal('createModal');
        loadFiles();
        
        // Clear inputs
        document.getElementById('newFileName').value = '';
        document.getElementById('newFileContent').value = '';
    } catch (error) {
        showNotification('Failed to create file', 'error');
    }
}

async function editFile(filename) {
    try {
        const data = await apiCall(`/api/view?file=${encodeURIComponent(filename)}`);
        
        state.currentEditFile = filename;
        document.getElementById('editFileName').textContent = filename;
        document.getElementById('editFileContent').value = data.content;
        
        openModal('editModal');
    } catch (error) {
        showNotification('Failed to load file', 'error');
    }
}

async function saveFile() {
    const content = document.getElementById('editFileContent').value;
    
    try {
        await apiCall('/api/edit', 'POST', {
            filename: state.currentEditFile,
            content,
            location: state.currentLocation
        });
        
        showNotification('File saved successfully!', 'success');
        closeModal('editModal');
        loadFiles();
    } catch (error) {
        showNotification('Failed to save file', 'error');
    }
}

async function viewFile(filename) {
    try {
        const data = await apiCall(`/api/view?file=${encodeURIComponent(filename)}`);
        
        document.getElementById('viewFileName').textContent = filename;
        document.getElementById('viewFileContent').textContent = data.content;
        
        openModal('viewModal');
    } catch (error) {
        showNotification('Failed to load file', 'error');
    }
}

async function deleteFile(filename) {
    if (!confirm(`Are you sure you want to delete "${filename}"?`)) {
        return;
    }
    
    try {
        await apiCall('/api/delete', 'POST', { 
            filename,
            location: state.currentLocation
        });
        showNotification('File deleted successfully!', 'success');
        loadFiles();
    } catch (error) {
        showNotification('Failed to delete file', 'error');
    }
}

async function checkFileExists(filename, location) {
    try {
        const data = await apiCall(`/api/exists?file=${encodeURIComponent(filename)}&location=${encodeURIComponent(location)}`);
        return data.exists;
    } catch (error) {
        return false;
    }
}

async function confirmOverwrite() {
    const filename = document.getElementById('newFileName').value.trim();
    const content = document.getElementById('newFileContent').value;
    const location = document.getElementById('fileLocation').value;
    
    closeModal('overwriteModal');
    await performCreateFile(filename, content, location);
}

// ============================================================================
// Code Execution - ENHANCED WITH BETTER OUTPUT
// ============================================================================
function openExecuteModal(filename) {
    state.currentExecuteFile = filename;
    document.getElementById('executeFileName').textContent = filename;
    document.getElementById('executionOutput').textContent = '';
    openModal('executeModal');
}

async function executeFile(action) {
    const outputElement = document.getElementById('executionOutput');
    outputElement.textContent = 'Executing...\n';
    
    // Disable buttons during execution
    document.getElementById('compileBtn').disabled = true;
    document.getElementById('runBtn').disabled = true;
    document.getElementById('compileRunBtn').disabled = true;
    
    try {
        const data = await apiCall('/api/execute', 'POST', {
            filename: state.currentExecuteFile,
            action,
            location: state.currentLocation
        });
        
        if (data.success) {
            outputElement.textContent = '✓ Execution completed successfully!\n\n';
            outputElement.textContent += '═══════════ OUTPUT ═══════════\n\n';
            outputElement.textContent += data.output || '(No output)';
            outputElement.textContent += '\n\n═══════════════════════════════';
            outputElement.textContent += `\nExit Code: ${data.exitCode || 0}`;
        } else {
            outputElement.textContent = '✗ Execution failed!\n\n';
            outputElement.textContent += '═══════════ ERROR ═══════════\n\n';
            outputElement.textContent += data.output || data.error || 'Unknown error';
            outputElement.textContent += '\n\n═══════════════════════════════';
            outputElement.textContent += `\nExit Code: ${data.exitCode || 1}`;
        }
    } catch (error) {
        outputElement.textContent = '✗ Execution failed!\n\n';
        outputElement.textContent += 'Error: ' + error.message;
    } finally {
        // Re-enable buttons
        document.getElementById('compileBtn').disabled = false;
        document.getElementById('runBtn').disabled = false;
        document.getElementById('compileRunBtn').disabled = false;
    }
}

// ============================================================================
// Location Browser
// ============================================================================
async function openLocationBrowser() {
    try {
        const data = await apiCall(`/api/browse?path=${encodeURIComponent(state.currentLocation)}`);
        displayDirectoryTree(data.directories, data.currentPath);
        openModal('locationModal');
    } catch (error) {
        showNotification('Failed to browse directories', 'error');
    }
}

function displayDirectoryTree(directories, currentPath) {
    const breadcrumb = document.getElementById('breadcrumb');
    const directoryList = document.getElementById('directoryList');
    
    // Update breadcrumb
    breadcrumb.textContent = currentPath;
    
    // Display directories
    directoryList.innerHTML = `
        <div class="directory-item" onclick="navigateToParent()">
            📁 ../ (Parent Directory)
        </div>
        ${directories.map(dir => `
            <div class="directory-item" onclick="navigateToDirectory('${dir}')">
                📁 ${dir}
            </div>
        `).join('')}
    `;
}

async function navigateToDirectory(dirName) {
    const newPath = state.currentLocation === '.' ? dirName : `${state.currentLocation}/${dirName}`;
    state.currentLocation = newPath;
    await openLocationBrowser();
}

async function navigateToParent() {
    const parts = state.currentLocation.split('/');
    parts.pop();
    state.currentLocation = parts.length > 0 ? parts.join('/') : '.';
    await openLocationBrowser();
}

function selectLocation() {
    document.getElementById('fileLocation').value = state.currentLocation;
    showNotification('Location updated!', 'success');
    closeModal('locationModal');
}

// ============================================================================
// Search and Filter
// ============================================================================
function handleSearch(e) {
    const query = e.target.value.toLowerCase();
    
    if (!query) {
        displayFiles(state.files);
        return;
    }
    
    const filtered = state.files.filter(file => 
        file.name.toLowerCase().includes(query)
    );
    
    displayFiles(filtered);
}

// ============================================================================
// View Toggle
// ============================================================================
function toggleView(view) {
    state.viewMode = view;
    
    // Update active button
    document.querySelectorAll('.view-btn').forEach(btn => {
        btn.classList.toggle('active', btn.dataset.view === view);
    });
    
    // Update grid class
    const fileGrid = document.getElementById('fileGrid');
    if (view === 'list') {
        fileGrid.style.gridTemplateColumns = '1fr';
    } else {
        fileGrid.style.gridTemplateColumns = 'repeat(auto-fill, minmax(280px, 1fr))';
    }
}

// ============================================================================
// Navigation
// ============================================================================
function handleNavigation(section) {
    // Update active nav item
    document.querySelectorAll('.nav-item').forEach(item => {
        item.classList.toggle('active', item.dataset.section === section);
    });
    
    // Filter files based on section
    let filteredFiles = state.files;
    
    switch(section) {
        case 'code':
            filteredFiles = state.files.filter(f => f.isCode);
            break;
        case 'recent':
            filteredFiles = state.files.slice(0, 5);
            break;
        case 'files':
        default:
            filteredFiles = state.files;
    }
    
    displayFiles(filteredFiles);
}

// ============================================================================
// Modal Management
// ============================================================================
function openModal(modalId) {
    const modal = document.getElementById(modalId);
    modal.classList.add('active');
    document.body.style.overflow = 'hidden';
}

function closeModal(modalId) {
    const modal = document.getElementById(modalId);
    modal.classList.remove('active');
    document.body.style.overflow = 'auto';
}

// ============================================================================
// Notifications
// ============================================================================
function showNotification(message, type = 'info') {
    // Create notification element
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        padding: 16px 24px;
        background: ${type === 'success' ? '#10b981' : type === 'error' ? '#ef4444' : type === 'warning' ? '#f59e0b' : '#3b82f6'};
        color: white;
        border-radius: 12px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.2);
        z-index: 10000;
        animation: slideIn 0.3s ease;
        font-weight: 600;
    `;
    notification.textContent = message;
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.style.animation = 'slideOut 0.3s ease';
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

// ============================================================================
// Utility Functions - ENHANCED WITH MORE FILE TYPES
// ============================================================================
function getFileExtension(filename) {
    const parts = filename.split('.');
    return parts.length > 1 ? parts[parts.length - 1] : 'file';
}

function getFileIcon(filename) {
    const ext = getFileExtension(filename).toLowerCase();
    const iconMap = {
        // Programming languages
        'js': '📜',
        'ts': '📘',
        'py': '🐍',
        'java': '☕',
        'cpp': '⚙️',
        'cc': '⚙️',
        'cxx': '⚙️',
        'c': '⚙️',
        'h': '📋',
        'hpp': '📋',
        'cs': '🎯',
        'go': '🔵',
        'rs': '🦀',
        'rb': '💎',
        'php': '🐘',
        'swift': '🔶',
        'kt': '🟣',
        'dart': '🎯',
        'lua': '🌙',
        'pl': '🐪',
        'r': '📊',
        // Web
        'html': '🌐',
        'css': '🎨',
        'scss': '🎨',
        'sass': '🎨',
        'less': '🎨',
        'vue': '💚',
        'jsx': '⚛️',
        'tsx': '⚛️',
        // Data & Config
        'json': '📋',
        'xml': '📋',
        'yaml': '📋',
        'yml': '📋',
        'toml': '📋',
        'ini': '⚙️',
        'conf': '⚙️',
        'config': '⚙️',
        // Documents
        'md': '📝',
        'txt': '📄',
        'pdf': '📕',
        'doc': '📘',
        'docx': '📘',
        // Shell
        'sh': '🔧',
        'bash': '🔧',
        'zsh': '🔧',
        'fish': '🐠',
        'bat': '🔧',
        'cmd': '🔧',
        'ps1': '🔧',
        // Database
        'sql': '🗄️',
        'db': '🗄️',
        'sqlite': '🗄️',
        // Other
        'gitignore': '🔒',
        'env': '🔐',
        'dockerfile': '🐳',
        'makefile': '🔨'
    };
    
    return iconMap[ext] || '📄';
}

function isCodeFile(filename) {
    const ext = getFileExtension(filename).toLowerCase();
    const codeExtensions = [
        'js', 'ts', 'py', 'java', 'cpp', 'cc', 'cxx', 'c', 'cs', 
        'go', 'rs', 'rb', 'php', 'swift', 'kt', 'dart', 'sh', 
        'bash', 'zsh', 'lua', 'pl', 'r', 'jsx', 'tsx'
    ];
    return codeExtensions.includes(ext);
}

// Add animation keyframes
const style = document.createElement('style');
style.textContent = `
    @keyframes slideIn {
        from {
            transform: translateX(400px);
            opacity: 0;
        }
        to {
            transform: translateX(0);
            opacity: 1;
        }
    }
    
    @keyframes slideOut {
        from {
            transform: translateX(0);
            opacity: 1;
        }
        to {
            transform: translateX(400px);
            opacity: 0;
        }
    }
`;
document.head.appendChild(style);
