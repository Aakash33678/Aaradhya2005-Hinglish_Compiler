import { LANGUAGES } from '../constants/languages';

export default function StatusBar({ language, isRunning, lastRunTime }) {
  const lang = LANGUAGES.find(l => l.id === language);

  return (
    <div
      className="flex items-center justify-between px-5 py-1.5 text-xs"
      style={{
        background: 'var(--bg-secondary)',
        borderTop: '1px solid var(--border)',
        color: 'var(--text-muted)',
        fontFamily: 'JetBrains Mono, monospace',
      }}
    >
      <div className="flex items-center gap-4">
        <span style={{ color: 'var(--accent)' }}>⬡ CodeForge IDE</span>
        <span>UTF-8</span>
        <span>Spaces: 4</span>
      </div>

      <div className="flex items-center gap-4">
        {isRunning && (
          <span style={{ color: 'var(--yellow)' }} className="flex items-center gap-1.5">
            <span className="inline-block w-1.5 h-1.5 rounded-full" style={{
              background: 'var(--yellow)',
              animation: 'pulse-dot 1s ease-in-out infinite',
            }} />
            Running...
          </span>
        )}
        {lastRunTime && !isRunning && (
          <span style={{ color: 'var(--green)' }}>✓ Finished in {lastRunTime}ms</span>
        )}
        <span>{lang?.icon} {lang?.label}</span>
        <span style={{ color: 'var(--text-muted)' }}>Ln 1, Col 1</span>
      </div>
    </div>
  );
}
