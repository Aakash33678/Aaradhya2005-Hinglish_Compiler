import Editor from '@monaco-editor/react';
import { LANGUAGES } from '../constants/languages';

const monacoTheme = {
  base: 'vs-dark',
  inherit: true,
  rules: [
    { token: 'comment', foreground: '555568', fontStyle: 'italic' },
    { token: 'keyword', foreground: 'a78bfa' },
    { token: 'string', foreground: '86efac' },
    { token: 'number', foreground: 'fbbf24' },
    { token: 'type', foreground: '67e8f9' },
    { token: 'function', foreground: '7dd3fc' },
    { token: 'variable', foreground: 'e8e8f0' },
    { token: 'operator', foreground: 'a78bfa' },
    { token: 'delimiter', foreground: '8888a0' },
  ],
  colors: {
    'editor.background': '#0d0d0f',
    'editor.foreground': '#e8e8f0',
    'editor.lineHighlightBackground': '#18181d',
    'editor.selectionBackground': '#7c6af730',
    'editor.inactiveSelectionBackground': '#7c6af720',
    'editorLineNumber.foreground': '#333345',
    'editorLineNumber.activeForeground': '#7c6af7',
    'editorCursor.foreground': '#7c6af7',
    'editorWidget.background': '#111114',
    'editorSuggestWidget.background': '#111114',
    'editorSuggestWidget.border': '#2a2a35',
    'editorSuggestWidget.selectedBackground': '#7c6af720',
    'editor.wordHighlightBackground': '#7c6af718',
    'editor.findMatchBackground': '#fbbf2440',
    'editor.findMatchHighlightBackground': '#fbbf2420',
    'scrollbarSlider.background': '#2a2a3580',
    'scrollbarSlider.hoverBackground': '#3a3a4880',
    'scrollbar.shadow': '#00000000',
    'minimap.background': '#0d0d0f',
  },
};

function beforeMount(monaco) {
  monaco.editor.defineTheme('codeforge', monacoTheme);
}

export default function CodeEditor({ code, language, onChange, onRun }) {
  const lang = LANGUAGES.find(l => l.id === language);

  function handleEditorDidMount(editor, monaco) {
    if (!onRun) return;

    editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter, () => {
      onRun();
    });
  }

  return (
    <div className="flex flex-col h-full" style={{ background: 'var(--bg-primary)' }}>
      {/* Tab bar */}
      <div
        className="flex items-center gap-1 px-3 pt-2"
        style={{ background: 'var(--bg-secondary)', borderBottom: '1px solid var(--border)' }}
      >
        <div
          style={{
            background: 'var(--bg-primary)',
            borderTop: '2px solid var(--accent)',
            color: 'var(--text-primary)',
            border: '1px solid var(--border)',
            borderBottom: 'none',
          }}
          className="flex items-center gap-2 px-4 py-2 text-xs rounded-t-md"
        >
          <span>{lang?.icon}</span>
          <span style={{ fontFamily: 'JetBrains Mono, monospace' }}>
            main.{language === 'python' ? 'py' : language === 'java' ? 'java' : language === 'javascript' ? 'js' : language}
          </span>
          <div
            className="w-1.5 h-1.5 rounded-full ml-1"
            style={{ background: 'var(--accent)', boxShadow: '0 0 4px var(--accent)' }}
          />
        </div>
      </div>

      {/* Editor */}
      <div className="flex-1 editor-container overflow-hidden">
        <Editor
          height="100%"
          language={lang?.monacoLang || 'cpp'}
          value={code}
          onChange={val => onChange(val || '')}
          onMount={handleEditorDidMount}
          theme="codeforge"
          beforeMount={beforeMount}
          options={{
            fontSize: 14,
            fontFamily: '"JetBrains Mono", "Fira Code", monospace',
            fontLigatures: true,
            lineHeight: 22,
            minimap: { enabled: false },
            scrollBeyondLastLine: false,
            renderLineHighlight: 'line',
            cursorBlinking: 'smooth',
            cursorSmoothCaretAnimation: 'on',
            smoothScrolling: true,
            padding: { top: 16, bottom: 16 },
            tabSize: 4,
            wordWrap: 'on',
            bracketPairColorization: { enabled: true },
            guides: {
              bracketPairs: true,
              indentation: true,
            },
            suggest: { preview: true },
            inlineSuggest: { enabled: true },
            formatOnPaste: true,
            scrollbar: {
              verticalScrollbarSize: 6,
              horizontalScrollbarSize: 6,
            },
          }}
        />
      </div>
    </div>
  );
}
