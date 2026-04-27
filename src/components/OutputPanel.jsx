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

const SpinnerRing = () => (
  <div className="flex flex-col items-center gap-4 py-8">
    <div className="relative w-12 h-12">
      <div
        className="spinner absolute inset-0 rounded-full border-2"
        style={{ borderColor: 'var(--accent-dim)', borderTopColor: 'var(--accent)' }}
      />
      <div className="absolute inset-2 rounded-full" style={{ background: 'var(--accent-dim)' }} />
    </div>
    <div className="text-xs tracking-widest uppercase" style={{ color: 'var(--text-muted)' }}>
      Executing...
    </div>
    <div className="flex gap-1.5">
      {[0, 1, 2].map(i => (
        <div
          key={i}
          className="w-1.5 h-1.5 rounded-full"
          style={{
            background: 'var(--accent)',
            animation: 'pulse-dot 1.2s ease-in-out infinite',
            animationDelay: `${i * 0.2}s`,
          }}
        />
      ))}
    </div>
  </div>
);

export default function OutputPanel({ output, error, isRunning, executionTime, title = 'output', emptyMessage = 'Run your code to see output' }) {
  const [copied, setCopied] = useState(false);

  const hasContent = output || error;

  const handleCopy = () => {
    navigator.clipboard.writeText(output || error || '');
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const statusColor = error ? 'var(--red)' : output ? 'var(--green)' : 'var(--text-muted)';
  const statusLabel = error ? 'Error' : output ? 'Success' : 'Idle';

  return (
    <div
      className="flex flex-col h-full"
      style={{ background: 'var(--bg-secondary)', border: '1px solid var(--border)', borderRadius: '10px', overflow: 'hidden' }}
    >
      {/* Header */}
      <div
        className="flex items-center justify-between px-4 py-2.5"
        style={{ borderBottom: '1px solid var(--border)', background: 'var(--bg-tertiary)' }}
      >
        <div className="flex items-center gap-3">
          <div className="flex gap-1">
            <div className="w-2.5 h-2.5 rounded-full" style={{ background: '#ff5f57' }} />
            <div className="w-2.5 h-2.5 rounded-full" style={{ background: '#febc2e' }} />
            <div className="w-2.5 h-2.5 rounded-full" style={{ background: '#28c840' }} />
          </div>
          <span className="text-xs font-semibold tracking-widest uppercase" style={{ color: 'var(--text-muted)' }}>
            {title}
          </span>
          <div className="flex items-center gap-1.5">
            <div
              className="w-1.5 h-1.5 rounded-full"
              style={{
                background: statusColor,
                boxShadow: `0 0 6px ${statusColor}`,
                transition: 'background 0.3s',
              }}
            />
            <span className="text-xs" style={{ color: statusColor, transition: 'color 0.3s' }}>
              {statusLabel}
            </span>
          </div>
        </div>

        <div className="flex items-center gap-2">
          {executionTime != null && (
            <span
              className="text-xs px-2 py-0.5 rounded"
              style={{
                color: 'var(--text-muted)',
                background: 'var(--bg-hover)',
                fontFamily: 'JetBrains Mono, monospace',
              }}
            >
              {executionTime}ms
            </span>
          )}

          {hasContent && !isRunning && (
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
      </div>

      {/* Content */}
      <div className="flex-1 overflow-auto p-4">
        {isRunning ? (
          <SpinnerRing />
        ) : error ? (
          <div className="animate-slide-up">
            {/* Error badge */}
            <div
              className="flex items-center gap-2 mb-3 px-3 py-2 rounded-lg text-xs font-semibold"
              style={{ background: 'rgba(248,113,113,0.1)', border: '1px solid rgba(248,113,113,0.25)', color: 'var(--red)' }}
            >
              <svg width="14" height="14" viewBox="0 0 14 14" fill="currentColor">
                <path d="M7 1C3.69 1 1 3.69 1 7s2.69 6 6 6 6-2.69 6-6-2.69-6-6-6zm.75 9H6.25V8.5h1.5V10zm0-3H6.25V4h1.5v3z" />
              </svg>
              Compilation / Runtime Error
            </div>
            <pre
              style={{
                fontFamily: '"JetBrains Mono", monospace',
                fontSize: '12.5px',
                color: '#fca5a5',
                lineHeight: '1.7',
                whiteSpace: 'pre-wrap',
                wordBreak: 'break-word',
              }}
            >
              {error}
            </pre>
          </div>
        ) : output ? (
          <pre
            className="animate-slide-up"
            style={{
              fontFamily: '"JetBrains Mono", monospace',
              fontSize: '13px',
              color: 'var(--text-primary)',
              lineHeight: '1.75',
              whiteSpace: 'pre-wrap',
              wordBreak: 'break-word',
            }}
          >
            {output}
          </pre>
        ) : (
          <div className="flex flex-col items-center justify-center h-full gap-3 opacity-40">
            <svg width="40" height="40" viewBox="0 0 40 40" fill="none" stroke="var(--text-muted)" strokeWidth="1.5">
              <rect x="4" y="8" width="32" height="24" rx="3" />
              <path d="M4 14H36M14 14V32" strokeLinecap="round" />
              <path d="M20 22l4 3-4 3M10 19l-4 3 4 3" strokeLinecap="round" strokeLinejoin="round" />
            </svg>
            <span className="text-sm" style={{ color: 'var(--text-muted)' }}>
              {emptyMessage}
            </span>
          </div>
        )}
      </div>
    </div>
  );
}
