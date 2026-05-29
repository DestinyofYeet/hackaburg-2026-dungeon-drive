import { BrainCircuit, ShieldAlert, Sparkles, Target } from 'lucide-react';

function inferThreatLevel(recommendation = '', narration = '') {
  const text = `${recommendation} ${narration}`.toLowerCase();

  if (text.includes('critical') || text.includes('dragon') || text.includes('boss')) {
    return 'Critical';
  }

  if (text.includes('enemy') || text.includes('goblin') || text.includes('attack')) {
    return 'Medium';
  }

  return 'Low';
}

function tacticalReason(recommendation = '') {
  if (recommendation.toLowerCase().includes('block')) {
    return 'This move controls the enemy path and protects the party from losing board position.';
  }

  if (recommendation.toLowerCase().includes('move')) {
    return 'This move improves positioning and prepares the party for the next tactical decision.';
  }

  return 'The AI is evaluating the board state and suggesting the safest next action.';
}

export default function AiPanel({ recommendation, narration }) {
  const threatLevel = inferThreatLevel(recommendation, narration);
  const reason = tacticalReason(recommendation);

  return (
    <section className="panel ai-panel">
      <div className="ai-panel-header">
        <div>
          <p className="eyebrow">AI Dungeon Master</p>
          <h3>Arcane Tactical Engine</h3>
        </div>
        <span className={`threat-badge ${threatLevel.toLowerCase()}`}>
          {threatLevel} Threat
        </span>
      </div>

      <div className="ai-block">
        <BrainCircuit size={22} />
        <div>
          <small>Recommended Move</small>
          <p>{recommendation}</p>
        </div>
      </div>

      <div className="ai-block compact">
        <Target size={19} />
        <div>
          <small>Tactical Reason</small>
          <p>{reason}</p>
        </div>
      </div>

      <div className="narration">
        <Sparkles size={18} />
        <div>
          <small>Live Narration</small>
          <p>{narration}</p>
        </div>
      </div>
    </section>
  );
}