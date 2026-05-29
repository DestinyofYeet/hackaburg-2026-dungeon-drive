# Dungeon Drive

A hybrid tabletop RPG platform that blends the tactile feel of miniatures with a smart digital layer. Magnetic position tracking with unique magnet signatures identifies each figurine, while an under-board actuator moves magnets to physically reposition pieces on command. The system automates movement, health, and status tracking so the DM’s actions trigger real-world motion in real time.

## Core Idea

- Physical implementation of D&D
- Combine pros of physical and digital playstyle
- DM can take actions and the figurines will get moved automatically
- Automatic health tracking, movement etc.
- Digital layer used for tracking + movement
- Magnets used for position tracking
- XY Gantry below the playing field to move the magnet sensor (at first), later we can just move another magnet, this is for combating the small range (~1cm) of the magnet sensor
- For the figurine distinction we use different magnet placings or strengths

---

## Tech stack

### Hardware

- XY Gantry
    - Motors: https://www.tme.eu/de/details/motans060020056laa/e-motoren/igus/mot-an-s-060-020-056-l-a-aaaa/
- Infineon PSOC Edge E84 AI Kit
- Infineon TLE493D-W2B6
- Raspberry Pi 5
- Infineon Stepper Motor Control Shield https://www.infineon.com/assets/row/public/documents/30/44/infineon-stepper-motor-control-shield-with-ifx9201sg-xmc1300-for-arduino-um-um-en.pdf?fileId=5546d462617643590161c23fa5120aa1

### Software

- Frontend: React
- Sensors, Motors: Arduino C

### Software add-on

- AI for recommendation
- Image Recognition of Dice number rolled using PSOC Edge

---

## Current Tasks

| Hannes | Figure out XY Gantry |
| --- | --- |
| Ole | Figure out Magnet sensor |
| Stefan | Figure out Infineon Microcontroller |
| Nooshin | React for the tracker |
| Farida |  |
| Andrej |  |

---

## Board scale and layout

- Grid: 1 square ≈ 1.5 m in-game; diagonals also count as 1.5 m per square.
- Creature space: Small/Medium 1×1 (≈1.5 m × 1.5 m); Large 2×2 (≈3 m × 3 m); Huge 3×3 (≈4.5 m × 4.5 m); Gargantuan 4×4+ (≈6 m × 6 m+).
- Reach: most creatures ≈1.5 m; reach weapons/monsters commonly ≈3 m (some are longer, e.g., ≈4.5 m+).
- Exact conversions if needed: 5 ft = 1.524 m, but tabletop uses the rounded 1.5 m per square for simplicity.

---

## D&D Basics AI Summary

Roles and flow

- Dungeon Master (DM): describes the world, plays NPCs/monsters, sets Difficulty Classes (DCs), adjudicates rules.
- Players: each run a character with goals, stats, gear, and abilities.
- Core loop: DM describes → players declare actions → roll dice if the outcome is uncertain → DM narrates results.
- Three pillars: exploration, social interaction, combat.

Core mechanics (5e-style)

- Roll d20 + modifier vs a DC (usually 10–20). Meet or beat the DC to succeed.
- Ability scores: STR, DEX, CON, INT, WIS, CHA; each gives a modifier (e.g., 16 = +3).
- Proficiency bonus: starts at +2, increases with level; add it to checks you’re proficient in (skills, saves, attacks).
- Advantage/Disadvantage: roll 2d20, take higher/lower.
- Types of rolls:
    - Ability checks (skills): pick the relevant ability/skill; DM sets DC.
    - Saving throws: resist harmful effects; each class has proficiencies in some saves.
    - Attack rolls vs Armor Class (AC): d20 + attack bonus vs target AC; on hit, roll damage dice.
- Criticals: natural 20 is a crit (double damage dice); natural 1 is an automatic miss.
- Conditions (common ones): prone, restrained, grappled, invisible, stunned, frightened—each has specific effects.

Combat basics

- Time: rounds of ~6 seconds; everyone takes a turn each round; order set by Initiative (d20 + DEX).
- On your turn: Move (up to your speed), take 1 Action, possibly 1 Bonus Action (if you have one), and 1 Reaction outside your turn when triggered.
- Common actions: Attack, Cast a Spell, Dash, Disengage, Dodge, Help, Hide, Ready, Search, Use an Object.
- Opportunity Attacks: leaving a creature’s reach without Disengage usually triggers a reaction attack.
- Cover: half (+2 to AC/DEX saves), three-quarters (+5), total cover can’t be targeted.
- Hit points & healing: track damage; heal via spells, potions, rests.
- Rests: Short Rest (~~1 hour) spend Hit Dice to heal; Long Rest (~~8 hours) restores more resources.
- Death saves: at 0 HP, roll d20 each turn; 3 successes stabilize, 3 failures die (20 = up at 1 HP; 1 = 2 fails).

Spellcasting basics

- Spell slots: power budget per level; regain on rests (varies by class).
- Spell save DC: 8 + proficiency + key ability; targets roll saves vs this DC.
- Concentration: only one concentration spell at a time; taking damage forces a CON save (DC 10 or half damage).

Characters and advancement

- Create: choose ancestry/species, class, background, and ability scores (point buy/standard array or roll).
- Equipment and features come from class/background; level up to gain features, spells, and better proficiency bonus.

---

### Frontend - How to run

Download and install Node.js from: **https://nodejs.org**

Then:

```powershell
cd frontend
npm install
npm run dev
```

---

### Backend

tbd

---

## Raspberry Credentials

Hostname: dungeondrive

Username: dd

Password: 123

SSH over Techbase Guest

**IP may need to be set to static!**

---
