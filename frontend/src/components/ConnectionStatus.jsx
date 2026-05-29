import { Activity, Radio, WifiOff } from 'lucide-react';

function statusText(connection) {
  if (connection.status === 'live') return 'WebSocket Live';
  if (connection.status === 'connecting') return 'Connecting';
  if (connection.status === 'mock') return 'Demo Mode';
  if (connection.status === 'error') return 'Connection Error';
  return 'Disconnected';
}

export default function ConnectionStatus({ connection, stats, compact = false }) {
  const isLive = connection.status === 'live';
  const isConnecting = connection.status === 'connecting';

  const telemetry = [
    {
      label: 'WebSocket',
      value: statusText(connection),
      active: isLive
    },
    {
      label: 'Raspberry Pi Bridge',
      value: isLive ? 'Receiving State' : 'Ready',
      active: isLive || isConnecting
    },
    {
      label: 'Magnetic Sensors',
      value: isLive ? 'Active Stream' : 'Simulated',
      active: isLive
    },
    {
      label: 'Tracked Pieces',
      value: stats?.tracked ?? 0,
      active: (stats?.tracked ?? 0) > 0
    }
  ];

  return (
    <aside className={`telemetry-card ${isLive ? 'live' : 'standby'} ${compact ? 'compact' : ''}`}>
      <div className="telemetry-header">
        <div className="telemetry-icon">
          {isLive ? <Radio size={20} /> : <WifiOff size={20} />}
        </div>
        <div>
          <p className="eyebrow">Magnetic Sensor Network</p>
          <strong>{isLive ? 'Live Hardware Feed' : 'Hardware Standby'}</strong>
          <span>
            {connection.lastSeen
              ? `Last sync ${connection.lastSeen.toLocaleTimeString()}`
              : 'Waiting for Raspberry Pi WebSocket'}
          </span>
        </div>
      </div>

      <div className="telemetry-grid">
        {telemetry.map((item) => (
          <div className="telemetry-row" key={item.label}>
            <span className={`telemetry-dot ${item.active ? 'active' : ''}`} />
            <div>
              <small>{item.label}</small>
              <strong>{item.value}</strong>
            </div>
          </div>
        ))}
      </div>

      <div className="telemetry-footer">
        <Activity size={15} />
        <span>{isLive ? 'Streaming board state to digital twin' : 'Demo-safe fallback enabled'}</span>
      </div>
    </aside>
  );
}