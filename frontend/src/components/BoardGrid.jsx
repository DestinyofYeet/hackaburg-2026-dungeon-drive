import { motion } from 'framer-motion';
import { Swords, Shield, Sparkles, Gem } from 'lucide-react';

const icons = {
  player: Shield,
  enemy: Swords,
  neutral: Gem,
  default: Sparkles
};

function labelFor(x, y) {
  return `${String.fromCharCode(65 + x)}${y + 1}`;
}

export default function BoardGrid({ board, figures, selectedId, onSelect, onMoveFigure }) {
  const width = board?.width || 10;
  const height = board?.height || 10;
  const cells = Array.from({ length: width * height }, (_, index) => ({
    x: index % width,
    y: Math.floor(index / width)
  }));

  return (
    <section className="board-shell">
      <div className="board-header">
        <div>
          <p className="eyebrow">Digital Twin</p>
          <h2>Magnetic Battle Board</h2>
        </div>
        <span>{width} × {height} grid</span>
      </div>

      <div className="board-grid" style={{ gridTemplateColumns: `repeat(${width}, minmax(0, 1fr))` }}>
        {cells.map((cell) => (
          <button
            type="button"
            className="board-cell"
            key={`${cell.x}-${cell.y}`}
            onClick={() => {
              if (selectedId && onMoveFigure) {
                onMoveFigure(selectedId, cell.x, cell.y);
              }
            }}
            title={`Move selected figure to ${labelFor(cell.x, cell.y)}`}
          >
            <span>{labelFor(cell.x, cell.y)}</span>
          </button>
        ))}

        <div className="figure-layer">
          {figures.map((figure) => {
            const Icon = icons[figure.team] || icons.default;
            const selected = selectedId === figure.id;

            const left = `${((figure.x + 0.5) / width) * 100}%`;
            const top = `${((figure.y + 0.5) / height) * 100}%`;

            return (
              <motion.button
                key={figure.id}
                className={`figure-token ${figure.team} ${selected ? 'selected' : ''}`}
                initial={{ scale: 0.5, opacity: 0 }}
                animate={{
                  left,
                  top,
                  scale: selected ? 1.12 : 1,
                  opacity: 1
                }}
                transition={{
                  left: { type: 'spring', stiffness: 120, damping: 18 },
                  top: { type: 'spring', stiffness: 120, damping: 18 },
                  scale: { duration: 0.18 }
                }}
                whileHover={{ scale: 1.16 }}
                onClick={(event) => {
                  event.stopPropagation();
                  onSelect(figure.id);
                }}
                title={`${figure.name} at ${labelFor(figure.x, figure.y)}`}
              >
                <Icon size={20} />
              </motion.button>
            );
          })}
        </div>
      </div>
    </section>
  );
}