import { useMemo, useState } from 'react';
import { Cpu, Gamepad2, RadioTower, Swords } from 'lucide-react';
import { useBoardSocket } from './hooks/useBoardSocket.js';
import BoardGrid from './components/BoardGrid.jsx';
import ConnectionStatus from './components/ConnectionStatus.jsx';
import CreaturePanel from './components/CreaturePanel.jsx';
import AiPanel from './components/AiPanel.jsx';
import EventLog from './components/EventLog.jsx';

const DEFAULT_SOCKET_URL = import.meta.env.VITE_BOARD_SOCKET_URL || 'ws://localhost:8000/ws';
const DEFAULT_API_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8000';

export default function App() {
  const [socketUrl, setSocketUrl] = useState(DEFAULT_SOCKET_URL);
  const [draftUrl, setDraftUrl] = useState(DEFAULT_SOCKET_URL);
  const [selectedId, setSelectedId] = useState('enemy_1');
  const [commandStatus, setCommandStatus] = useState('');
  const { boardState, connection, stats } = useBoardSocket(socketUrl);

  const selected = useMemo(() => {
    return boardState.figures?.find((figure) => figure.id === selectedId) || boardState.figures?.[0];
  }, [boardState.figures, selectedId]);

  const moveFigure = async (figureId, x, y) => {
    setCommandStatus(`Sending move command for ${figureId}...`);

    try {
      const response = await fetch(`${DEFAULT_API_URL}/api/figures/${figureId}/move`, {
        method: 'PATCH',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ x, y })
      });

      if (!response.ok) {
        throw new Error(`Move request failed (${response.status})`);
      }

      setCommandStatus(`Move confirmed for ${figureId} → ${x}, ${y}`);
    } catch (error) {
      setCommandStatus(error.message);
    }
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

      <section className="overview-grid">
        <div className="overview-main">
          <section className="socket-card">
            <div className="socket-controls">
              <div>
                <p className="eyebrow">Raspberry Pi WebSocket</p>
                <strong>{socketUrl || 'No socket connected — showing polished mock data'}</strong>
              </div>
              <form onSubmit={(event) => { event.preventDefault(); setSocketUrl(draftUrl.trim()); }}>
                <input value={draftUrl} onChange={(event) => setDraftUrl(event.target.value)} placeholder="ws://localhost:8000/ws" />
                <button type="submit">Connect</button>
                <button type="button" className="ghost" onClick={() => setSocketUrl('')}>Demo Mode</button>
              </form>
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
    </main>
  );
}
