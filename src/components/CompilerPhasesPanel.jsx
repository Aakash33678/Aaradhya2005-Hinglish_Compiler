import { useState } from 'react';

const CopyIcon = ({ copied }) => copied ? (
  <svg width="13" height="13" viewBox="0 0 13 13" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round">
    <path d="M2 7L5 10L11 3" />
  </svg>
) : (
  <svg width="13" height="13" viewBox="0 0 13 13" fill="none" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round">
    <rect x="4" y="4" width="8" height="8" rx="1.5" />
    <path d="M9 4V2.5A1.5 1.5 0 007.5 1h-5A1.5 1.5 0 001 2.5v5A1.5 1.5 0 002.5 9H4" />
  </svg>
);

export default function CompilerPhasesPanel({ content, isRunning }) {
  const [copied, setCopied] = useState(false);

  const handleCopy = () => {
    navigator.clipboard.writeText(content || '');
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  return (
    <div
      className="flex flex-col h-full"
      style={{ background: 'var(--bg-secondary)', border: '1px solid var(--border)', borderRadius: '10px', overflow: 'hidden' }}
    >
      <div
        className="flex items-center justify-between px-4 py-2.5"
        style={{ borderBottom: '1px solid var(--border)', background: 'var(--bg-tertiary)' }}
      >
        <div className="flex items-center gap-2">
          <div className="flex gap-1">
            <div className="w-2.5 h-2.5 rounded-full" style={{ background: '#ff5f57' }} />
            <div className="w-2.5 h-2.5 rounded-full" style={{ background: '#febc2e' }} />
            <div className="w-2.5 h-2.5 rounded-full" style={{ background: '#28c840' }} />
          </div>
          <span className="text-xs font-semibold tracking-widest uppercase" style={{ color: 'var(--text-muted)' }}>
            compiler phases
          </span>
        </div>

        {content && !isRunning && (
          <button
            onClick={handleCopy}
            className="flex items-center gap-1.5 text-xs px-2.5 py-1 rounded transition-all duration-200"
            style={{
              color: copied ? 'var(--green)' : 'var(--text-muted)',
              background: copied ? 'rgba(74,222,128,0.1)' : 'var(--bg-hover)',
              border: `1px solid ${copied ? 'rgba(74,222,128,0.3)' : 'transparent'}`,
            }}
          >
            <CopyIcon copied={copied} />
            {copied ? 'Copied!' : 'Copy'}
          </button>
        )}
      </div>

      <div className="flex-1 overflow-auto p-4">
        {isRunning ? (
          <div className="text-sm" style={{ color: 'var(--text-muted)' }}>Collecting compiler phases...</div>
        ) : content ? (
          <pre
            className="animate-slide-up"
            style={{
              fontFamily: '"JetBrains Mono", monospace',
              fontSize: '12.5px',
              color: 'var(--text-primary)',
              lineHeight: '1.7',
              whiteSpace: 'pre-wrap',
              wordBreak: 'break-word',
            }}
          >
            {content}
          </pre>
        ) : (
          <div className="text-sm" style={{ color: 'var(--text-muted)' }}>
            Compiler phase details will appear here after execution.
          </div>
        )}
      </div>
    </div>
  );
}