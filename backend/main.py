"""FastAPI backend for the Arcane Board Command Center.

WebSocket endpoint: ws://<host>:8000/ws
  - Clients receive the full BoardState as JSON on connect and after every mutation.
  - The Raspberry Pi driver can POST raw board data to /api/driver/sync.

REST endpoints:
  GET    /api/state                    – current board state
  POST   /api/figures                  – create a figure
  DELETE /api/figures/{id}             – remove a figure
  PATCH  /api/figures/{id}/move        – move figure to (x, y)
  PATCH  /api/figures/{id}/hp          – update figure HP
  PATCH  /api/turn                     – update turn info
  PATCH  /api/ai                       – update AI recommendation / narration
  POST   /api/events                   – append an event message
  POST   /api/driver/sync              – bulk-replace state (Raspberry Pi driver)
"""
from __future__ import annotations

from contextlib import asynccontextmanager
import json

from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware

from models import (
    AddEventRequest,
    BoardState,
    CreateFigureRequest,
    Figure,
    MoveFigureRequest,
    UpdateAiRequest,
    UpdateHpRequest,
    UpdateTurnRequest,
)
from state import game_state


@asynccontextmanager
async def lifespan(app: FastAPI):  # noqa: ARG001
    yield


app = FastAPI(title="Arcane Board API", version="1.0.0", lifespan=lifespan)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)


# ── helpers ──────────────────────────────────────────────────────────────────


def _figure_or_404(figure_id: str) -> Figure:
    figure = game_state.get_figure(figure_id)
    if figure is None:
        raise HTTPException(status_code=404, detail=f"Figure '{figure_id}' not found")
    return figure


# ── WebSocket ─────────────────────────────────────────────────────────────────


@app.websocket("/ws")
async def websocket_endpoint(ws: WebSocket) -> None:
    await ws.accept()
    game_state.connect(ws)
    # Send full state immediately on connect
    await ws.send_text(game_state.state.to_ws_json())
    try:
        while True:
            frame = await ws.receive()

            if frame.get("type") == "websocket.disconnect":
                raise WebSocketDisconnect()

            message_text = frame.get("text")
            if message_text is None and frame.get("bytes") is not None:
                try:
                    message_text = frame["bytes"].decode("utf-8")
                except UnicodeDecodeError:
                    continue

            if not message_text:
                continue

            try:
                message = json.loads(message_text)
            except json.JSONDecodeError:
                # Ignore non-JSON messages so malformed clients don't crash the socket.
                continue

            if message.get("type") == "servo_command":
                angle = message.get("angle")
                if isinstance(angle, (int, float)) and 0 <= angle <= 180:
                    payload = json.dumps({"type": "driver_servo_command", "angle": angle})
                    for client in list(game_state._connections):
                        try:
                            await client.send_text(payload)
                        except Exception:
                            pass
                continue

            if message.get("type") != "move_command":
                # Ignore unknown message types.
                continue

            command_id = message.get("command_id")
            figure_id = message.get("figure_id")
            x = message.get("x")
            y = message.get("y")

            # Validate command_id
            if not isinstance(command_id, str) or not command_id:
                await ws.send_text(json.dumps({
                    "type": "command_error",
                    "command_id": command_id or None,
                    "error": "Missing or invalid command_id"
                }))
                continue

            # Validate figure_id
            if not isinstance(figure_id, str) or not figure_id:
                await ws.send_text(json.dumps({
                    "type": "command_error",
                    "command_id": command_id,
                    "error": "Missing or invalid figure_id"
                }))
                continue

            # Validate x/y are integers
            try:
                x_int = int(x)
                y_int = int(y)
            except (TypeError, ValueError):
                await ws.send_text(json.dumps({
                    "type": "command_error",
                    "command_id": command_id,
                    "error": "x and y must be integers"
                }))
                continue

            # Validate figure exists
            figure = game_state.get_figure(figure_id)
            if figure is None:
                await ws.send_text(json.dumps({
                    "type": "command_error",
                    "command_id": command_id,
                    "error": f"Figure '{figure_id}' not found"
                }))
                continue

            # Validate x/y in bounds
            board = game_state.state.board
            if not (0 <= x_int < board.width and 0 <= y_int < board.height):
                await ws.send_text(json.dumps({
                    "type": "command_error",
                    "command_id": command_id,
                    "error": f"Target ({x_int}, {y_int}) out of bounds"
                }))
                continue

            # Command accepted: send ack to sender
            await ws.send_text(json.dumps({
                "type": "command_ack",
                "command_id": command_id,
                "figure_id": figure_id,
                "x": x_int,
                "y": y_int
            }))

            # Broadcast driver_move_command for hardware integration
            driver_move_msg = json.dumps({
                "type": "driver_move_command",
                "command_id": command_id,
                "figure_id": figure_id,
                "x": x_int,
                "y": y_int
            })
            for client in list(game_state._connections):
                try:
                    await client.send_text(driver_move_msg)
                except Exception:
                    pass

            game_state.add_event(f"Move command accepted: {figure_id} → ({x_int}, {y_int})")

            # Demo behavior: move the digital figure immediately.
            game_state.move_figure(figure_id, x_int, y_int)
            await game_state.broadcast()
    except WebSocketDisconnect:
        pass
    finally:
        game_state.disconnect(ws)


# ── REST: read ────────────────────────────────────────────────────────────────


@app.get("/api/state", response_model=BoardState)
async def get_state() -> BoardState:
    return game_state.state


# ── REST: figures ─────────────────────────────────────────────────────────────


@app.post("/api/figures", response_model=Figure, status_code=201)
async def create_figure(body: CreateFigureRequest) -> Figure:
    figure = Figure(**body.model_dump(by_alias=False))
    try:
        game_state.add_figure(figure)
    except ValueError as exc:
        raise HTTPException(status_code=409, detail=str(exc)) from exc
    await game_state.broadcast()
    return figure


@app.delete("/api/figures/{figure_id}", status_code=200)
async def delete_figure(figure_id: str) -> None:
    _figure_or_404(figure_id)
    game_state.remove_figure(figure_id)
    await game_state.broadcast()


@app.patch("/api/figures/{figure_id}/move", response_model=Figure)
async def move_figure(figure_id: str, body: MoveFigureRequest) -> Figure:
    _figure_or_404(figure_id)
    figure = game_state.move_figure(figure_id, body.x, body.y)
    await game_state.broadcast()
    return figure


@app.patch("/api/figures/{figure_id}/hp", response_model=Figure)
async def update_hp(figure_id: str, body: UpdateHpRequest) -> Figure:
    _figure_or_404(figure_id)
    figure = game_state.update_hp(figure_id, body.hp)
    await game_state.broadcast()
    return figure


# ── REST: turn ────────────────────────────────────────────────────────────────


@app.patch("/api/turn")
async def update_turn(body: UpdateTurnRequest) -> dict:
    turn = game_state.update_turn(body.round, body.active, body.phase)
    await game_state.broadcast()
    return turn.model_dump()


# ── REST: AI ──────────────────────────────────────────────────────────────────


@app.patch("/api/ai")
async def update_ai(body: UpdateAiRequest) -> dict:
    game_state.update_ai(body.recommendation, body.narration)
    await game_state.broadcast()
    return {
        "recommendation": game_state.state.recommendation,
        "narration": game_state.state.narration,
    }


# ── REST: events ──────────────────────────────────────────────────────────────


@app.post("/api/events", status_code=201)
async def add_event(body: AddEventRequest) -> dict:
    game_state.add_event(body.message)
    await game_state.broadcast()
    return {"message": body.message}


# ── REST: driver sync (Raspberry Pi) ─────────────────────────────────────────


@app.post("/api/driver/sync")
async def driver_sync(body: BoardState) -> dict:
    """Accept a full board state update from the Raspberry Pi driver."""
    game_state.replace_state(body)
    await game_state.broadcast()
    return {"ok": True}
