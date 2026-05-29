import { useState } from 'react';

const lessons = [
  {
    title: 'What is Dungeons & Dragons?',
    body: 'D&D is a cooperative storytelling game. Players control heroes, while the Dungeon Master controls the world, monsters, and story.'
  },
  {
    title: 'Meet the Heroes',
    body: 'Each hero has a character sheet: name, class, HP, skills, attacks, and spells. Open a sheet to see what a D&D character looks like.',
    links: [
      ['Official Character Sheets', 'https://www.dndbeyond.com/resources/1779-d-d-character-sheets'],
      ['Character Builder', 'https://www.dndbeyond.com/characters'],
      ['Printable 2024 Sheet', 'https://media.dndbeyond.com/compendium-images/phb/downloads/DnD_2024_Character-Sheet.pdf']
    ],
    dice: 'Dice decide what happens. A d20 is used for most success checks. Damage can use dice like d4, d6, d8, d10, or d12.'
  },
  {
    title: 'How Turns Work',
    body: 'On a turn, a hero can usually move, take an action, maybe use a bonus action, and then end the turn.'
  },
  {
    title: 'Why Our Board Is Different',
    body: 'Normally players move miniatures by hand. Our board connects software, Raspberry Pi, magnetic movement, and a live digital twin.'
  },
  {
    title: "You're Ready!",
    body: 'You now understand the basics: heroes, dice, turns, AI recommendations, and autonomous board movement.'
  }
];

export default function LearnDnd({ onLaunch }) {
  const [index, setIndex] = useState(0);
  const lesson = lessons[index];
  const progress = ((index + 1) / lessons.length) * 100;

  return (
    <section className="learn-shell">
      <div className="learn-card">
        <p className="eyebrow">2-Minute Crash Course</p>
        <h2>{lesson.title}</h2>
        <p>{lesson.body}</p>

        {lesson.links && (
          <div className="learn-links">
            <strong>Character sheet links:</strong>
            {lesson.links.map(([label, url]) => (
              <a key={url} href={url} target="_blank" rel="noreferrer">
                {label}
              </a>
            ))}
          </div>
        )}

        {lesson.dice && (
          <div className="dice-box">
            <strong>Dice basics</strong>
            <p>{lesson.dice}</p>
          </div>
        )}

        <div className="progress-track">
          <div style={{ width: `${progress}%` }} />
        </div>

        <div className="learn-actions">
          <button disabled={index === 0} onClick={() => setIndex(index - 1)}>
            Back
          </button>

          {index < lessons.length - 1 ? (
            <button onClick={() => setIndex(index + 1)}>Next</button>
          ) : (
            <button onClick={onLaunch}>Launch Command Center</button>
          )}
        </div>
      </div>
    </section>
  );
}