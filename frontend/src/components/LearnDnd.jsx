export default function LearnDnd() {
  return (
    <section className="panel learn-dnd">
      <h2>Learn D&D</h2>
      <p className="muted">
        Quick crash course for hackathon players.
      </p>

      <div className="learn-grid">
        <article className="learn-card">
          <h3>Goal of the Game</h3>
          <p>
            Dungeons & Dragons is cooperative storytelling. Players control heroes,
            the Dungeon Master controls the world, and dice decide uncertain outcomes.
          </p>
        </article>

        <article className="learn-card">
          <h3>Turn Basics</h3>
          <p>
            On your turn you usually get movement, one action, and optional bonus actions.
            Positioning on the board is often as important as attacking.
          </p>
        </article>

        <article className="learn-card">
          <h3>Common Actions</h3>
          <p>
            Attack, Cast a Spell, Dash, Dodge, Help, and Hide are the most common choices.
            Pick actions that support team strategy, not only damage.
          </p>
        </article>

        <article className="learn-card">
          <h3>For This Demo</h3>
          <p>
            Click a creature token, then click a target cell to send a move command.
            The board reflects live digital state while hardware integration catches up.
          </p>
        </article>
      </div>
    </section>
  );
}
