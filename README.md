# Hinglish Compiler IDE

A React frontend for the Hinglish compiler, backed by a Flask API that runs the C++ compiler logic in `backend/main.cpp`.

## ✨ Features

- 🎨 **Dark theme** — deep, refined color palette inspired by VS Code
- 🖊️ **Monaco Editor** — full syntax highlighting, line numbers, bracket matching, autocomplete
- 🌐 **Hinglish compiler** — runs `.li` code through the C++ backend in `backend/main.cpp`
- ⌨️ **Keyboard shortcut** — `Ctrl+Enter` to run code
- 📥 **Custom stdin** — dedicated input panel
- 📤 **Output console** — shows stdout, compilation errors, runtime errors
- ⏱️ **Execution time** — displayed after each run
- 📋 **Copy output** — one-click copy button
- 🌀 **Loading spinner** — smooth animation while executing
- 🔴 **Error display** — compilation/runtime errors shown clearly in red
- 📊 **Status bar** — language, encoding, run status

## 📁 Folder Structure

```
compiler-ide/
├── index.html
├── package.json
├── vite.config.js
├── tailwind.config.js
├── postcss.config.js
├── .env.example
└── src/
    ├── main.jsx
    ├── App.jsx
    ├── index.css
    ├── constants/
    │   └── languages.js          # Language configs + default code snippets
    ├── services/
    │   └── api.js                # Axios API call to backend
    └── components/
        ├── Navbar.jsx            # Top bar: logo, language selector, run button
        ├── CodeEditor.jsx        # Monaco Editor with custom theme
        ├── InputPanel.jsx        # stdin textarea
        ├── OutputPanel.jsx       # stdout/stderr display + copy button
        └── StatusBar.jsx         # Bottom status bar
```

## 🚀 Getting Started

### Prerequisites
- Node.js >= 18
- npm or yarn
- Python 3.10+
- g++ available on your PATH

### Installation

```bash
# Install dependencies
npm install

# Install backend dependency
pip install -r backend/requirements.txt

# Start the frontend
npm run dev

# In a second terminal, start the Flask backend
npm run backend
```

Open [http://localhost:5173](http://localhost:5173) in your browser.

### Build for Production

```bash
npm run build
npm run preview
```

## 🔌 Backend API

The frontend sends a POST request to `http://localhost:5000/run` with Hinglish source code:

```json
{
  "code": "<user_code>",
  "language": "hinglish"
}
```

Note: this compiler currently does not support runtime stdin input.

Expected response shapes:

```json
{ "output": "...compiler output..." }
{ "stdout": "...compiler output..." }
{ "error": "...parse or semantic error..." }
{ "stdout": "...", "stderr": "..." }
```

To change the backend URL, edit `.env`:
```
VITE_API_URL=http://your-backend-url
```

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+Enter` | Run code |
| `Ctrl+/` | Toggle comment (Monaco) |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` | Redo |
