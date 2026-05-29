"""In-memory game state with async WebSocket broadcast."""
from __future__ import annotations

from typing import Optional, Set

from fastapi import WebSocket

from models import Board, BoardState, Figure, Team, Turn

_DEFAULT_FIGURES = [
    Figure(id="hero_1", name="Knight", role="Tank", team=Team.player, x=2, y=6, hp=18, maxHp=20, status="guarding"),
    Figure(id="hero_2", name="Mage", role="Caster", team=Team.player, x=3, y=7, hp=9, maxHp=14, status="focused"),
    Figure(id="enemy_1", name="Goblin Scout", role="Enemy", team=Team.enemy, x=6, y=3, hp=7, maxHp=10, status="moving"),
    Figure(id="npc_1", name="Ancient Relic", role="Objective", team=Team.neutral, x=8, y=8, hp=1, maxHp=1, status="active"),
]

_DEFAULT_EVENTS = [
    "Goblin Scout detected at G4.",
    "Knight moved from B6 to C7.",
    "Mage is within spell range of the relic.",
    "Magnetic board sync initialized.",
]

MAX_EVENTS = 50


class GameState:
    def __init__(self) -> None:
        self._state = BoardState(
            board=Board(width=10, height=10),
            turn=Turn(round=3, active="Goblin Scout", phase="Enemy Movement"),
            figures=list(_DEFAULT_FIGURES),
            events=list(_DEFAULT_EVENTS),
            recommendation=(
                "Move the Knight one cell north-west to block the Goblin path "
                "while the Mage captures the Ancient Relic."
            ),
            narration=(
                "A pulse of blue light ripples through the board as the hidden magnet "
                "awakens the battlefield."
            ),
        )
        self._connections: Set[WebSocket] = set()

    # ── state access ────────────────────────────────────────────────────────

    @property
    def state(self) -> BoardState:
        return self._state

    def get_figure(self, figure_id: str) -> Optional[Figure]:
        return next((f for f in self._state.figures if f.id == figure_id), None)

    # ── mutations ────────────────────────────────────────────────────────────

    def move_figure(self, figure_id: str, x: int, y: int) -> Figure:
        figure = self.get_figure(figure_id)
        if figure is None:
            raise KeyError(figure_id)
        figure.x = x
        figure.y = y
        return figure

    def update_hp(self, figure_id: str, hp: int) -> Figure:
        figure = self.get_figure(figure_id)
        if figure is None:
            raise KeyError(figure_id)
        figure.hp = min(hp, figure.max_hp)
        return figure

    def add_figure(self, figure: Figure) -> None:
        if self.get_figure(figure.id) is not None:
            raise ValueError(f"Figure '{figure.id}' already exists")
        self._state.figures.append(figure)

    def remove_figure(self, figure_id: str) -> None:
        figure = self.get_figure(figure_id)
        if figure is None:
            raise KeyError(figure_id)
        self._state.figures.remove(figure)

    def add_event(self, message: str) -> None:
        self._state.events.append(message)
        if len(self._state.events) > MAX_EVENTS:
            self._state.events = self._state.events[-MAX_EVENTS:]

    def update_turn(self, round_: Optional[int], active: Optional[str], phase: Optional[str]) -> Turn:
        if round_ is not None:
            self._state.turn.round = round_
        if active is not None:
            self._state.turn.active = active
        if phase is not None:
            self._state.turn.phase = phase
        return self._state.turn

    def update_ai(self, recommendation: Optional[str], narration: Optional[str]) -> None:
        if recommendation is not None:
            self._state.recommendation = recommendation
        if narration is not None:
            self._state.narration = narration

    def replace_state(self, new_state: BoardState) -> None:
        """Bulk-replace the entire board state (e.g., from Raspberry Pi driver)."""
        self._state = new_state

    # ── WebSocket connections ────────────────────────────────────────────────

    def connect(self, ws: WebSocket) -> None:
        self._connections.add(ws)

    def disconnect(self, ws: WebSocket) -> None:
        self._connections.discard(ws)

    async def broadcast(self) -> None:
        payload = self._state.to_ws_json()
        dead: Set[WebSocket] = set()
        for ws in list(self._connections):
            try:
                await ws.send_text(payload)
            except Exception:
                dead.add(ws)
        self._connections -= dead


game_state = GameState()
