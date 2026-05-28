import { useMemo, useState } from 'react';
import { Cpu, Gamepad2, RadioTower, Swords } from 'lucide-react';
import { useBoardSocket } from './hooks/useBoardSocket.js';
import BoardGrid from './components/BoardGrid.jsx';
import ConnectionStatus from './components/ConnectionStatus.jsx';
import CreaturePanel from './components/CreaturePanel.jsx';
import AiPanel from './components/AiPanel.jsx';
import EventLog from './components/EventLog.jsx';

const DEFAULT_SOCKET_URL = import.meta.env.VITE_BOARD_SOCKET_URL || '';

export default function App() {
  const [socketUrl, setSocketUrl] = useState(DEFAULT_SOCKET_URL);
  const [draftUrl, setDraftUrl] = useState(DEFAULT_SOCKET_URL || 'ws://raspberrypi.local:8000/ws');
  const [selectedId, setSelectedId] = useState('enemy_1');
  const { boardState, connection, stats } = useBoardSocket(socketUrl);

  const selected = useMemo(() => {
    return boardState.figures?.find((figure) => figure.id === selectedId) || boardState.figures?.[0];
  }, [boardState.figures, selectedId]);

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
        <ConnectionStatus connection={connection} />
      </header>

      <section className="socket-card">
        <div>
          <p className="eyebrow">Raspberry Pi WebSocket</p>
          <strong>{socketUrl || 'No socket connected — showing polished mock data'}</strong>
        </div>
        <form onSubmit={(event) => { event.preventDefault(); setSocketUrl(draftUrl.trim()); }}>
          <input value={draftUrl} onChange={(event) => setDraftUrl(event.target.value)} placeholder="ws://raspberrypi.local:8000/ws" />
          <button type="submit">Connect</button>
          <button type="button" className="ghost" onClick={() => setSocketUrl('')}>Demo Mode</button>
        </form>
      </section>

      <section className="stat-grid">
        <div className="stat-card"><Gamepad2 /><span>Tracked Pieces</span><strong>{stats.tracked}</strong></div>
        <div className="stat-card"><Swords /><span>Enemies</span><strong>{stats.enemies}</strong></div>
        <div className="stat-card"><Cpu /><span>Players</span><strong>{stats.players}</strong></div>
        <div className="stat-card"><RadioTower /><span>Turn Phase</span><strong>{boardState.turn?.phase || 'Live Sync'}</strong></div>
      </section>

      <div className="layout-grid">
        <BoardGrid board={boardState.board} figures={boardState.figures || []} selectedId={selected?.id} onSelect={setSelectedId} />
        <aside className="side-stack">
          <CreaturePanel selected={selected} />
          <AiPanel recommendation={boardState.recommendation} narration={boardState.narration} />
          <EventLog events={boardState.events} />
        </aside>
      </div>
    </main>
  );
}
