import { useMemo, useState } from 'react';
import { Cpu, Gamepad2, RadioTower, Swords } from 'lucide-react';
import { useBoardSocket } from './hooks/useBoardSocket.js';
import BoardGrid from './components/BoardGrid.jsx';
import ConnectionStatus from './components/ConnectionStatus.jsx';
import CreaturePanel from './components/CreaturePanel.jsx';
import AiPanel from './components/AiPanel.jsx';
import EventLog from './components/EventLog.jsx';
import LearnDnd from './components/LearnDnd.jsx';


const DEFAULT_BACKEND_IP = '127.0.0.1';
const DEFAULT_SOCKET_URL = `ws://${DEFAULT_BACKEND_IP}:8000/ws`;

export default function App() {
  const [socketUrl, setSocketUrl] = useState(DEFAULT_SOCKET_URL);
  const [backendIp, setBackendIp] = useState(DEFAULT_BACKEND_IP);
  const [draftUrl, setDraftUrl] = useState(
    DEFAULT_BACKEND_IP ? `ws://${DEFAULT_BACKEND_IP}:8000/ws` : DEFAULT_SOCKET_URL
  );
  const [selectedId, setSelectedId] = useState('enemy_1');
  const [commandStatus, setCommandStatus] = useState('');
  const [activeTab, setActiveTab] = useState('command');
  const { boardState, connection, stats, sendMoveCommand, sendServoCommand } = useBoardSocket(socketUrl);

  const selected = useMemo(() => {
    return boardState.figures?.find((figure) => figure.id === selectedId) || boardState.figures?.[0];
  }, [boardState.figures, selectedId]);

  const moveFigure = (figureId, x, y) => {
    setCommandStatus(`Sending physical move command for ${figureId}...`);

    const sent = sendMoveCommand(figureId, x, y);

    if (!sent) {
      setCommandStatus('Move command failed: WebSocket is not connected');
      return;
    }

    setCommandStatus(`Move command queued for ${figureId} → ${x}, ${y}`);
  };

  return (
    <main className="app-shell">
      <div className="background-glow" />
      <header className="hero">
        <div>
          <p className="eyebrow">HackaBurg 2026 Prototype</p>
          <h1>Arcane Board Command Center</h1>
          <p className="hero-copy">
            A live digital twin for a magnetic Dungeons & Dragons board, powered by Raspberry Pi data and ready for AI-assisted gameplay.
          </p>
        </div>
      </header>

      <div className="app-tabs">
        <button
          type="button"
          className={activeTab === 'command' ? 'active' : ''}
          onClick={() => setActiveTab('command')}
        >
          Command Center
        </button>
        <button
          type="button"
          className={activeTab === 'learn' ? 'active' : ''}
          onClick={() => setActiveTab('learn')}
        >
          Learn D&D
        </button>
      </div>

      {activeTab === 'learn' ? (
        <LearnDnd onLaunch={() => { setActiveTab('command'); setDraftUrl(`ws://${window.location.origin}:8000/ws`); }} />
      ) : (
        <>
          <section className="overview-grid">
            <div className="overview-main">
              <section className="socket-card" onLoad={() => {setDraftUrl(`ws://${window.location.origin}:8000/ws`)} }>
                <div className="socket-controls">
                  <div>
                    <p className="eyebrow">Raspberry Pi WebSocket</p>
                    <strong>{`Current IP: ${backendIp}`}</strong>
                  </div>
                  {commandStatus ? <span className="command-status">{commandStatus}</span> : null}
                </div>
              </section>

              <section className="stat-grid">
                <div className="stat-card"><Gamepad2 /><span>Tracked Pieces</span><strong>{stats.tracked}</strong></div>
                <div className="stat-card"><Swords /><span>Enemies</span><strong>{stats.enemies}</strong></div>
                <div className="stat-card"><Cpu /><span>Players</span><strong>{stats.players}</strong></div>
                <div className="stat-card"><RadioTower /><span>Turn Phase</span><strong>{boardState.turn?.phase || 'Live Sync'}</strong></div>
              </section>
            </div>

            <ConnectionStatus connection={connection} stats={stats} compact />
          </section>

          <div className="layout-grid">
            <BoardGrid board={boardState.board} figures={boardState.figures || []} selectedId={selected?.id} onSelect={setSelectedId} onMoveFigure={moveFigure} />
            <aside className="side-stack">
              <CreaturePanel selected={selected} />
              <AiPanel recommendation={boardState.recommendation} narration={boardState.narration} />
              <EventLog events={boardState.events} />
            </aside>
          </div>
        </>
      )}
    </main>
  );
}
