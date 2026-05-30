from __future__ import annotations

from enum import Enum
from typing import List, Literal, Optional

from pydantic import BaseModel, Field


class Team(str, Enum):
    player = "player"
    enemy = "enemy"
    neutral = "neutral"


class Figure(BaseModel):
    id: str
    name: str
    role: str
    team: Team
    x: int = Field(ge=0)
    y: int = Field(ge=0)
    hp: int = Field(ge=0)
    max_hp: int = Field(alias="maxHp", ge=1)
    status: str = "idle"

    model_config = {"populate_by_name": True}


class Board(BaseModel):
    width: int = Field(default=10, ge=1)
    height: int = Field(default=10, ge=1)


class Turn(BaseModel):
    round: int = Field(default=1, ge=1)
    active: str = ""
    phase: str = "Setup"


class BoardState(BaseModel):
    type: Literal["board_state"] = "board_state"
    board: Board = Field(default_factory=Board)
    turn: Turn = Field(default_factory=Turn)
    figures: List[Figure] = Field(default_factory=list)
    events: List[str] = Field(default_factory=list)
    recommendation: str = ""
    narration: str = ""

    def to_ws_json(self) -> str:
        """Serialize to JSON with camelCase aliases for the frontend."""
        return self.model_dump_json(by_alias=True)


# ── Request bodies ──────────────────────────────────────────────────────────


class CreateFigureRequest(BaseModel):
    id: str
    name: str
    role: str
    team: Team
    x: int = Field(ge=0)
    y: int = Field(ge=0)
    hp: int = Field(ge=0)
    max_hp: int = Field(alias="maxHp", ge=1)
    status: str = "idle"

    model_config = {"populate_by_name": True}


class MoveFigureRequest(BaseModel):
    x: int = Field(ge=0)
    y: int = Field(ge=0)


class UpdateHpRequest(BaseModel):
    hp: int = Field(ge=0)


class AddEventRequest(BaseModel):
    message: str


class UpdateTurnRequest(BaseModel):
    round: Optional[int] = Field(default=None, ge=1)
    active: Optional[str] = None
    phase: Optional[str] = None


class UpdateAiRequest(BaseModel):
    recommendation: Optional[str] = None
    narration: Optional[str] = None


class SensorReading(BaseModel):
    x: float
    y: float
    z: float
