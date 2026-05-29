# Arcane Board – Python Backend

FastAPI backend for the **Dungeons & Dragons Magnetic Board** project.  
It maintains the live board state and streams it to the React frontend over WebSocket.

## Quick start

```bash
cd backend
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

Interactive API docs: <http://localhost:8000/docs>

## Endpoints

| Method | Path | Description |
|--------|------|-------------|
| `WS` | `/ws` | WebSocket – streams full `BoardState` on connect and after every mutation |
| `GET` | `/api/state` | Current board state (JSON) |
| `POST` | `/api/figures` | Create a new figure |
| `DELETE` | `/api/figures/{id}` | Remove a figure |
| `PATCH` | `/api/figures/{id}/move` | Move figure to `{x, y}` |
| `PATCH` | `/api/figures/{id}/hp` | Set figure HP |
| `PATCH` | `/api/turn` | Update round / active / phase |
| `PATCH` | `/api/ai` | Update AI recommendation and narration |
| `POST` | `/api/events` | Append an event log message |
| `POST` | `/api/driver/sync` | Bulk-replace state (Raspberry Pi driver) |

## WebSocket message schema

```jsonc
{
  "type": "board_state",
  "board": { "width": 10, "height": 10 },
  "turn": { "round": 3, "active": "Goblin Scout", "phase": "Enemy Movement" },
  "figures": [
    { "id": "hero_1", "name": "Knight", "role": "Tank", "team": "player",
      "x": 2, "y": 6, "hp": 18, "maxHp": 20, "status": "guarding" }
  ],
  "events": ["Goblin Scout detected at G4."],
  "recommendation": "Move Knight north-west ...",
  "narration": "A pulse of blue light ..."
}
```

## Raspberry Pi driver integration

POST the full board state to `/api/driver/sync` whenever the magnetic sensors
detect a piece movement. All connected WebSocket clients will receive the
update immediately.

```bash
curl -X POST http://raspberrypi.local:8000/api/driver/sync \
     -H 'Content-Type: application/json' \
     -d '{"board":{"width":10,"height":10},"figures":[...],...}'
```
