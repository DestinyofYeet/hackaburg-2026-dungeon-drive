# Arcane Board Command Center

A polished React/Vite UI for the HackaBurg magnetic D&D board prototype.

It is designed as a judge-facing demo cockpit:

- live digital twin board
- Raspberry Pi WebSocket connection
- tracked pieces on a grid
- selected creature details
- HP/status display
- AI Dungeon Master recommendation panel
- live event log
- mock fallback mode when hardware is not connected

## Run locally

```bash
npm install
npm run dev
```

Open the local Vite URL in your browser.

## Connect to the Raspberry Pi WebSocket

You can connect from the UI input, or create a `.env.local` file:

```bash
VITE_BOARD_SOCKET_URL=ws://raspberrypi.local:8000/ws
```

Then restart the dev server.

## Recommended WebSocket payload

The UI accepts `figures` or `entities`.

```json
{
  "type": "board_state",
  "board": { "width": 10, "height": 10 },
  "turn": { "round": 3, "active": "Goblin Scout", "phase": "Enemy Movement" },
  "figures": [
    {
      "id": "enemy_1",
      "name": "Goblin Scout",
      "role": "Enemy",
      "team": "enemy",
      "x": 6,
      "y": 3,
      "hp": 7,
      "maxHp": 10,
      "status": "moving"
    }
  ],
  "events": ["Goblin Scout moved to G4."],
  "recommendation": "Move the Knight to block the Goblin path.",
  "narration": "A magnetic pulse ripples across the battlefield."
}
```

Coordinates are zero-based:

- `x: 0, y: 0` = A1
- `x: 9, y: 9` = J10 on a 10x10 board

## Files

```text
src/App.jsx
src/hooks/useBoardSocket.js
src/components/BoardGrid.jsx
src/components/ConnectionStatus.jsx
src/components/CreaturePanel.jsx
src/components/AiPanel.jsx
src/components/EventLog.jsx
src/styles.css
```
