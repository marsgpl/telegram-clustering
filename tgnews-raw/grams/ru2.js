const fs = require('fs');

const INPUT = 'ru/2grams-3.txt';
const OUTPUT = '../../tgnews/grams/ru2grams.lua';

const file = fs.readFileSync(INPUT);

const freq_w1_w2 = [];

file.toString('utf-8').trim().split('\n').forEach(line => {
    let parts = line.split(/\s+/);

    let freq = Number(parts[0]);
    let w1 = parts[1];
    let w2 = parts[parts.length - 1];

    if (!w1) return;
    if (!w2) return;

    w1 = w1.toLowerCase();
    w2 = w2.toLowerCase();

    if (w1.match(/[^а-яё]/)) return;
    if (w2.match(/[^а-яё]/)) return;

    if (w1.length + w2.length < 5) return;

    freq_w1_w2.push([
        Number(freq),
        w1,
        w2,
    ]);
});

const result = [];

freq_w1_w2
    .sort((a, b) => a[0] == b[0] ? 0 : a[0] > b[0] ? -1 : 1)
    .slice(0, 50000)
    .forEach(([_, w1, w2]) => {
        result.push(`[" ${w1} ${w2} "]=1`);
    });

fs.writeFileSync(OUTPUT,
    'return {' + result.join(',') + '}');
