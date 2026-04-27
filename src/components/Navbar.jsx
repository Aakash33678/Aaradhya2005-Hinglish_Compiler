import { useState } from 'react';
import { LANGUAGES } from '../constants/languages';

const PlayIcon = () => (
  <svg width="14" height="14" viewBox="0 0 14 14" fill="currentColor">
    <path d="M2.5 1.5L11.5 7L2.5 12.5V1.5Z" />
  </svg>
);

const SpinnerIcon = () => (
  <svg className="spinner" width="14" height="14" viewBox="0 0 14 14" fill="none">
    <circle cx="7" cy="7" r="5.5" stroke="currentColor" strokeWidth="2" strokeLinecap="round"
      strokeDasharray="22" strokeDashoffset="8" />
  </svg>
);

const ChevronIcon = () => (
  <svg width="12" height="12" viewBox="0 0 12 12" fill="none" stroke="currentColor" strokeWidth="2">
    <path d="M3 4.5L6 7.5L9 4.5" strokeLinecap="round" strokeLinejoin="round" />
  </svg>
);

export default function Navbar({ language, onLanguageChange, onRun, isRunning }) {
  const [dropdownOpen, setDropdownOpen] = useState(false);
  const selected = LANGUAGES.find(l => l.id === language);

  return (
    <header
      style={{
        background: 'var(--bg-secondary)',
        borderBottom: '1px solid var(--border)',
      }}
      className="flex items-center justify-between px-5 py-3 z-50"
    >
      {/* Logo */}
      <div className="flex items-center gap-3">
        <div
          style={{
            background: 'linear-gradient(135deg, var(--accent), #a78bfa)',
            boxShadow: '0 0 16px var(--accent-glow)',
          }}
          className="w-8 h-8 rounded-lg flex items-center justify-center"
        >
          <svg width="16" height="16" viewBox="0 0 16 16" fill="white">
            <path d="M5 3L1 8L5 13M11 3L15 8L11 13M9 2L7 14" stroke="white" strokeWidth="1.5"
              strokeLinecap="round" strokeLinejoin="round" fill="none" />
          </svg>
        </div>
        <div>
          <span
            className="text-base font-bold tracking-tight"
            style={{ color: 'var(--text-primary)', fontFamily: 'Syne, sans-serif' }}
          >
            Code<span style={{ color: 'var(--accent)' }}>Forge</span>
          </span>
          <div className="text-xs" style={{ color: 'var(--text-muted)', lineHeight: 1 }}>
            Online IDE
          </div>
        </div>
      </div>

      {/* Center — Language Selector */}
      <div className="flex items-center gap-3">
        <div className="relative">
          <button
            onClick={() => setDropdownOpen(o => !o)}
            style={{
              background: 'var(--bg-tertiary)',
              border: '1px solid var(--border)',
              color: 'var(--text-primary)',
            }}
            className="flex items-center gap-2 px-3 py-2 rounded-lg text-sm font-medium transition-all hover:border-[var(--accent)] focus:outline-none"
          >
            <span className="text-base leading-none">{selected?.icon}</span>
            <span style={{ fontFamily: 'JetBrains Mono, monospace', fontSize: '13px' }}>
              {selected?.label}
            </span>
            <ChevronIcon />
          </button>

          {dropdownOpen && (
            <div
              style={{
                background: 'var(--bg-panel)',
                border: '1px solid var(--border)',
                boxShadow: '0 8px 24px rgba(0,0,0,0.4)',
              }}
              className="absolute top-full mt-1 left-0 w-44 rounded-lg overflow-hidden z-50 animate-fade-in"
            >
              {LANGUAGES.map(lang => (
                <button
                  key={lang.id}
                  onClick={() => { onLanguageChange(lang.id); setDropdownOpen(false); }}
                  style={{
                    background: lang.id === language ? 'var(--accent-dim)' : 'transparent',
                    color: lang.id === language ? 'var(--accent)' : 'var(--text-secondary)',
                    borderLeft: lang.id === language ? '2px solid var(--accent)' : '2px solid transparent',
                  }}
                  className="w-full flex items-center gap-2 px-3 py-2.5 text-sm text-left hover:bg-[var(--bg-hover)] transition-colors"
                >
                  <span>{lang.icon}</span>
                  <span style={{ fontFamily: 'JetBrains Mono, monospace', fontSize: '13px' }}>
                    {lang.label}
                  </span>
                </button>
              ))}
            </div>
          )}
        </div>

        {/* Keyboard shortcut hint */}
        <div
          className="hidden sm:flex items-center gap-1 text-xs px-2 py-1 rounded"
          style={{ background: 'var(--bg-tertiary)', color: 'var(--text-muted)', border: '1px solid var(--border-subtle)' }}
        >
          <kbd style={{ fontFamily: 'JetBrains Mono, monospace' }}>Ctrl</kbd>
          <span>+</span>
          <kbd style={{ fontFamily: 'JetBrains Mono, monospace' }}>↵</kbd>
        </div>
      </div>

      {/* Run Button */}
      <button
        onClick={onRun}
        disabled={isRunning}
        style={{
          background: isRunning
            ? 'var(--bg-tertiary)'
            : 'linear-gradient(135deg, var(--accent), #6c5ce7)',
          color: isRunning ? 'var(--text-muted)' : 'white',
          border: isRunning ? '1px solid var(--border)' : 'none',
          boxShadow: isRunning ? 'none' : '0 4px 14px var(--accent-glow)',
        }}
        className="run-btn-shine flex items-center gap-2 px-5 py-2 rounded-lg text-sm font-semibold transition-all duration-200 disabled:cursor-not-allowed hover:scale-105 active:scale-95"
      >
        {isRunning ? (
          <>
            <SpinnerIcon />
            <span>Running...</span>
          </>
        ) : (
          <>
            <PlayIcon />
            <span>Run Code</span>
          </>
        )}
      </button>
    </header>
  );
}
