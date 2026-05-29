import { useEffect, useMemo, useRef, useState } from 'react';
import { Clock3, ScrollText } from 'lucide-react';

const MAX_EVENTS = 120;

function classifyEvent(message) {
  const text = String(message || '').toLowerCase();

  if (text.includes('sensor') || text.includes('magnet')) {
    return { type: 'sensor', icon: '🧲' };
  }
  if (text.includes('ai') || text.includes('threat') || text.includes('classified')) {
    return { type: 'ai', icon: '🤖' };
  }
  if (text.includes('move') || text.includes('moved') || text.includes('entered') || text.includes('attack') || text.includes('dragon')) {
    return { type: 'movement', icon: '⚔️' };
  }
  if (text.includes('broadcast') || text.includes('sync') || text.includes('websocket') || text.includes('frontend') || text.includes('state')) {
    return { type: 'system', icon: '📡' };
  }

  return { type: 'system', icon: '✨' };
}

function formatClockTime(value) {
  if (!value) return new Date().toLocaleTimeString([], { hour12: false });

  const parsed = value instanceof Date ? value : new Date(value);
  if (!Number.isNaN(parsed.getTime())) {
    return parsed.toLocaleTimeString([], { hour12: false });
  }

  return String(value);
}

function normalizeEvent(event) {
  if (typeof event === 'string') {
    return { message: event.trim() };
  }

  if (event && typeof event === 'object') {
    return {
      message: String(event.message || event.text || event.event || '').trim(),
      timestamp: event.timestamp || event.time || event.at || null
    };
  }

  return { message: '' };
}

export default function EventLog({ events }) {
  const [timeline, setTimeline] = useState([]);
  const previousBatchRef = useRef([]);

  useEffect(() => {
    const incoming = (events || [])
      .map(normalizeEvent)
      .filter((entry) => entry.message.length > 0);

    if (incoming.length === 0) {
      previousBatchRef.current = [];
      return;
    }

    const previousBySignature = new Map();
    for (const entry of previousBatchRef.current) {
      const signature = `${entry.message}::${entry.timestamp || ''}`;
      previousBySignature.set(signature, (previousBySignature.get(signature) || 0) + 1);
    }

    const additions = [];
    for (const entry of incoming) {
      const signature = `${entry.message}::${entry.timestamp || ''}`;
      const seenCount = previousBySignature.get(signature) || 0;
      if (seenCount > 0) {
        previousBySignature.set(signature, seenCount - 1);
      } else {
        additions.push(entry);
      }
    }

    if (additions.length > 0) {
      const now = new Date();
      const newEntries = additions
        .map((entry, index) => {
          const { type, icon } = classifyEvent(entry.message);
          const timestamp = formatClockTime(entry.timestamp || new Date(now.getTime() + index));
          return {
            id: `${timestamp}-${entry.message}-${Math.random().toString(36).slice(2, 9)}`,
            message: entry.message,
            timestamp,
            type,
            icon
          };
        })
        .reverse();

      setTimeline((current) => [...newEntries, ...current].slice(0, MAX_EVENTS));
    }

    previousBatchRef.current = incoming;
  }, [events]);

  const displayEvents = useMemo(() => timeline, [timeline]);

  return (
    <section className="panel event-log">
      <p className="eyebrow">
        <ScrollText size={15} />
        Live System Timeline
      </p>

      <div className="events timeline-feed">
        {displayEvents.length === 0 && (
          <div className="event-empty">
            <Clock3 size={14} />
            <span>Awaiting incoming hardware and AI events...</span>
          </div>
        )}

        {displayEvents.map((event) => (
          <article className={`event-item timeline ${event.type}`} key={event.id}>
            <span className={`event-glow ${event.type}`} />
            <time className="event-time">{event.timestamp}</time>
            <div className="event-body">
              <span className="event-icon" aria-hidden="true">{event.icon}</span>
              <p>{event.message}</p>
            </div>
          </article>
        ))}
      </div>
    </section>
  );
}