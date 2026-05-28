import { useEffect, useMemo, useState } from 'react';

const mockState = {
  type: 'board_state',
  board: { width: 10, height: 10 },
  turn: { round: 3, active: 'Goblin Scout', phase: 'Enemy Movement' },
  figures: [
    { id: 'hero_1', name: 'Knight', role: 'Tank', team: 'player', x: 2, y: 6, hp: 18, maxHp: 20, status: 'guarding' },
    { id: 'hero_2', name: 'Mage', role: 'Caster', team: 'player', x: 3, y: 7, hp: 9, maxHp: 14, status: 'focused' },
    { id: 'enemy_1', name: 'Goblin Scout', role: 'Enemy', team: 'enemy', x: 6, y: 3, hp: 7, maxHp: 10, status: 'moving' },
    { id: 'npc_1', name: 'Ancient Relic', role: 'Objective', team: 'neutral', x: 8, y: 8, hp: 1, maxHp: 1, status: 'active' }
  ],
  events: [
    'Goblin Scout detected at G4.',
    'Knight moved from B6 to C7.',
    'Mage is within spell range of the relic.',
    'Magnetic board sync initialized.'
  ],
  recommendation: 'Move the Knight one cell north-west to block the Goblin path while the Mage captures the Ancient Relic.',
  narration: 'A pulse of blue light ripples through the board as the hidden magnet awakens the battlefield.'
};

function normalizeIncoming(data) {
  if (!data || typeof data !== 'object') return mockState;

  return {
    ...mockState,
    ...data,
    board: data.board || mockState.board,
    figures: data.figures || data.entities || mockState.figures,
    events: data.events || mockState.events,
    recommendation: data.recommendation || mockState.recommendation,
    narration: data.narration || mockState.narration
  };
}

export function useBoardSocket(socketUrl) {
  const [boardState, setBoardState] = useState(mockState);
  const [connection, setConnection] = useState({ status: 'mock', lastSeen: null, error: null });

  useEffect(() => {
    if (!socketUrl) {
      setConnection({ status: 'mock', lastSeen: null, error: null });
      return;
    }

    let socket;
    let cancelled = false;

    try {
      socket = new WebSocket(socketUrl);
      setConnection({ status: 'connecting', lastSeen: null, error: null });

      socket.onopen = () => {
        if (!cancelled) setConnection({ status: 'live', lastSeen: new Date(), error: null });
      };

      socket.onmessage = (event) => {
        try {
          const parsed = JSON.parse(event.data);
          if (!cancelled) {
            setBoardState(normalizeIncoming(parsed));
            setConnection({ status: 'live', lastSeen: new Date(), error: null });
          }
        } catch (error) {
          if (!cancelled) setConnection((current) => ({ ...current, error: 'Invalid JSON payload' }));
        }
      };

      socket.onerror = () => {
        if (!cancelled) setConnection({ status: 'error', lastSeen: null, error: 'WebSocket error' });
      };

      socket.onclose = () => {
        if (!cancelled) setConnection((current) => ({ ...current, status: 'disconnected' }));
      };
    } catch (error) {
      setConnection({ status: 'error', lastSeen: null, error: error.message });
    }

    return () => {
      cancelled = true;
      if (socket) socket.close();
    };
  }, [socketUrl]);

  const stats = useMemo(() => {
    const figures = boardState.figures || [];
    return {
      players: figures.filter((f) => f.team === 'player').length,
      enemies: figures.filter((f) => f.team === 'enemy').length,
      tracked: figures.length
    };
  }, [boardState.figures]);

  return { boardState, connection, stats };
}
