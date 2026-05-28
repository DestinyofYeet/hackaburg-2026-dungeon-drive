import { ScrollText } from 'lucide-react';

export default function EventLog({ events }) {
  return (
    <section className="panel event-log">
      <p className="eyebrow"><ScrollText size={15} /> Live Event Log</p>
      <div className="events">
        {(events || []).map((event, index) => (
          <div className="event-item" key={`${event}-${index}`}>
            <span>{String(index + 1).padStart(2, '0')}</span>
            <p>{event}</p>
          </div>
        ))}
      </div>
    </section>
  );
}
