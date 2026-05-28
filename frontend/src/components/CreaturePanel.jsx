import { Activity, HeartPulse, MapPin } from 'lucide-react';

export default function CreaturePanel({ selected }) {
  if (!selected) {
    return (
      <section className="panel empty-panel">
        <p className="eyebrow">Selection</p>
        <h3>No creature selected</h3>
        <p>Click a board token to inspect live position, HP, and tactical status.</p>
      </section>
    );
  }

  const hpPercent = Math.max(0, Math.min(100, (selected.hp / selected.maxHp) * 100));

  return (
    <section className="panel">
      <p className="eyebrow">Selected Entity</p>
      <h3>{selected.name}</h3>
      <p className="muted">{selected.role} · {selected.team}</p>

      <div className="metric-row">
        <span><MapPin size={16} /> Position</span>
        <strong>{String.fromCharCode(65 + selected.x)}{selected.y + 1}</strong>
      </div>
      <div className="metric-row">
        <span><Activity size={16} /> Status</span>
        <strong>{selected.status}</strong>
      </div>
      <div className="hp-box">
        <div className="metric-row">
          <span><HeartPulse size={16} /> HP</span>
          <strong>{selected.hp}/{selected.maxHp}</strong>
        </div>
        <div className="hp-track"><div style={{ width: `${hpPercent}%` }} /></div>
      </div>
    </section>
  );
}
