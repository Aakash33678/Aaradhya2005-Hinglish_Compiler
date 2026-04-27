import { useState, useCallback, useEffect } from 'react';
import Navbar from './components/Navbar';
import CodeEditor from './components/CodeEditor';
import CompilerPhasesPanel from './components/CompilerPhasesPanel';
import OutputPanel from './components/OutputPanel';
import StatusBar from './components/StatusBar';
import { DEFAULT_CODE } from './constants/languages';
import { runCode } from './services/api';

const OUTPUT_MARKER = '=== Output of Input Code ===';

function splitCompilerText(rawText) {
  if (!rawText) {
    return { programOutput: '', phasesOutput: '' };
  }

  const markerIndex = rawText.indexOf(OUTPUT_MARKER);
  if (markerIndex === -1) {
    return { programOutput: rawText.trim(), phasesOutput: '' };
  }

  const phasesOutput = rawText.slice(0, markerIndex).trim();
  const programOutput = rawText.slice(markerIndex + OUTPUT_MARKER.length).trim();

  return {
    programOutput,
    phasesOutput,
  };
}

export default function App() {
  const [language, setLanguage] = useState('hinglish');
  const [code, setCode] = useState(DEFAULT_CODE['hinglish']);
  const [output, setOutput] = useState('');
  const [phasesOutput, setPhasesOutput] = useState('');
  const [error, setError] = useState('');
  const [isRunning, setIsRunning] = useState(false);
  const [executionTime, setExecutionTime] = useState(null);
  const [lastRunTime, setLastRunTime] = useState(null);

  // Change language → load default snippet
  const handleLanguageChange = useCallback((lang) => {
    setLanguage(lang);
    setCode(DEFAULT_CODE[lang] || '');
    setOutput('');
    setError('');
    setExecutionTime(null);
    setLastRunTime(null);
  }, []);

  const handleRun = useCallback(async () => {
    if (isRunning) return;
    setIsRunning(true);
    setOutput('');
    setPhasesOutput('');
    setError('');
    setExecutionTime(null);

    const startTime = performance.now();

    try {
      const data = await runCode({ code, language });
      const elapsed = Math.round(performance.now() - startTime);
      setExecutionTime(elapsed);
      setLastRunTime(elapsed);

      // Handle various backend response shapes
      if (data.error) {
        const parsedError = splitCompilerText(data.error);
        setPhasesOutput(parsedError.phasesOutput || data.stdout || '');
        setError(data.error);
      } else if (data.output !== undefined) {
        const parsed = splitCompilerText(data.output);
        setOutput(parsed.programOutput);
        setPhasesOutput(parsed.phasesOutput);
      } else if (data.stdout !== undefined) {
        const parsed = splitCompilerText(data.stdout);
        setOutput(parsed.programOutput);
        setPhasesOutput(parsed.phasesOutput);
        if (data.stderr) setError(data.stderr);
      } else {
        setOutput(JSON.stringify(data, null, 2));
      }
    } catch (err) {
      const elapsed = Math.round(performance.now() - startTime);
      setExecutionTime(elapsed);
      setLastRunTime(elapsed);

      if (err.response?.data) {
        const d = err.response.data;
        setError(d.error || d.stderr || JSON.stringify(d, null, 2));
        if (d.stdout) {
          const parsed = splitCompilerText(d.stdout);
          setPhasesOutput(parsed.phasesOutput || d.stdout);
        }
      } else if (err.code === 'ECONNABORTED') {
        setError('Execution timed out (30s limit exceeded).');
      } else if (err.code === 'ERR_NETWORK' || err.message?.includes('Network')) {
        setError(
          'Cannot connect to the backend server.\n' +
          'Make sure your backend is running at http://localhost:5000'
        );
      } else {
        setError(err.message || 'An unexpected error occurred.');
      }
    } finally {
      setIsRunning(false);
    }
  }, [code, language, isRunning]);

  // Keyboard shortcut: Ctrl+Enter
  useEffect(() => {
    const handler = (e) => {
      if ((e.ctrlKey || e.metaKey) && (e.key === 'Enter' || e.code === 'NumpadEnter')) {
        e.preventDefault();
        handleRun();
      }
    };
    window.addEventListener('keydown', handler, true);
    return () => window.removeEventListener('keydown', handler, true);
  }, [handleRun]);

  return (
    <div
      className="flex flex-col"
      style={{ height: '100vh', background: 'var(--bg-primary)', overflow: 'hidden' }}
    >
      {/* Navbar */}
      <Navbar
        language={language}
        onLanguageChange={handleLanguageChange}
        onRun={handleRun}
        isRunning={isRunning}
      />

      {/* Main layout */}
      <div className="flex flex-1 overflow-hidden gap-3 p-3">
        {/* Left: Code Editor */}
        <div
          className="flex-1 min-w-0 overflow-hidden rounded-xl"
          style={{ border: '1px solid var(--border)', background: 'var(--bg-primary)' }}
        >
          <CodeEditor
            code={code}
            language={language}
            onChange={setCode}
            onRun={handleRun}
          />
        </div>

        {/* Right: Program Output + Compiler Phases */}
        <div
          className="flex flex-col gap-3"
          style={{ width: '38%', minWidth: '320px', maxWidth: '520px' }}
        >
          {/* Program Output */}
          <div style={{ height: '32%', minHeight: '120px' }}>
            <OutputPanel
              output={output}
              error={error}
              isRunning={isRunning}
              executionTime={executionTime}
              title="program output"
              emptyMessage="Run your code to see final output"
            />
          </div>

          {/* Compiler Phases */}
          <div className="flex-1 min-h-0">
            <CompilerPhasesPanel
              content={phasesOutput}
              isRunning={isRunning}
            />
          </div>
        </div>
      </div>

      {/* Status bar */}
      <StatusBar
        language={language}
        isRunning={isRunning}
        lastRunTime={lastRunTime}
      />
    </div>
  );
}
