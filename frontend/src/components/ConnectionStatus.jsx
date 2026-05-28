import { Radio, WifiOff } from 'lucide-react';

export default function ConnectionStatus({ connection }) {
  const isLive = connection.status === 'live';
  const label = isLive ? 'LIVE HARDWARE FEED' : connection.status === 'mock' ? 'MOCK DEMO MODE' : connection.status.toUpperCase();

  return (
    <div className={`connection-pill ${isLive ? 'live' : 'mock'}`}>
      {isLive ? <Radio size={18} /> : <WifiOff size={18} />}
      <div>
        <strong>{label}</strong>
        <span>{connection.lastSeen ? `Last update ${connection.lastSeen.toLocaleTimeString()}` : 'Waiting for Raspberry Pi WebSocket'}</span>
      </div>
    </div>
  );
}
