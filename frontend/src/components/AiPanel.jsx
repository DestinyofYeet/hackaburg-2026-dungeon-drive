import { BrainCircuit, Sparkles } from 'lucide-react';

export default function AiPanel({ recommendation, narration }) {
  return (
    <section className="panel ai-panel">
      <p className="eyebrow">AI Dungeon Master</p>
      <div className="ai-block">
        <BrainCircuit size={22} />
        <div>
          <h3>Recommended Action</h3>
          <p>{recommendation}</p>
        </div>
      </div>
      <div className="narration">
        <Sparkles size={18} />
        <p>{narration}</p>
      </div>
    </section>
  );
}
