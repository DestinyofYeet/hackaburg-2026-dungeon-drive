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

export default function BoardGrid({ board, figures, selectedId, onSelect }) {
  const width = board?.width || 10;
  const height = board?.height || 10;
  const cells = Array.from({ length: width * height }, (_, index) => ({ x: index % width, y: Math.floor(index / width) }));

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
          <div className="board-cell" key={`${cell.x}-${cell.y}`}>
            <span>{labelFor(cell.x, cell.y)}</span>
          </div>
        ))}

        {figures.map((figure) => {
          const Icon = icons[figure.team] || icons.default;
          const selected = selectedId === figure.id;
          return (
            <motion.button
              layout
              initial={{ scale: 0.5, opacity: 0 }}
              animate={{ scale: selected ? 1.12 : 1, opacity: 1 }}
              whileHover={{ scale: 1.1 }}
              className={`figure-token ${figure.team} ${selected ? 'selected' : ''}`}
              key={figure.id}
              style={{ gridColumn: figure.x + 1, gridRow: figure.y + 1 }}
              onClick={() => onSelect(figure.id)}
              title={`${figure.name} at ${labelFor(figure.x, figure.y)}`}
            >
              <Icon size={20} />
            </motion.button>
          );
        })}
      </div>
    </section>
  );
}
