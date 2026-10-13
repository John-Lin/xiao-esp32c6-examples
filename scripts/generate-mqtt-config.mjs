// Generate a private C++ header from the build environment, without logging secrets.
import { chmodSync, existsSync, writeFileSync } from 'node:fs';

const usage = 'Usage: node scripts/generate-mqtt-config.mjs <output-file>\n'
  + 'Requires WIFI_SSID, WIFI_PASSWORD (may be empty), and MQTT_HOST.';
const args = process.argv.slice(2);
if (args.length === 1 && args[0] === '--help') {
  console.log(usage);
  process.exit(0);
}
if (args.length !== 1) {
  console.error(usage);
  process.exit(1);
}

const settings = [
  ['WIFI_SSID', 'wifiSsid'],
  ['WIFI_PASSWORD', 'wifiPassword'],
  ['MQTT_HOST', 'mqttHost'],
];
const lines = [
  '// Generated from the build environment. Contains credentials; do not commit.',
  '#pragma once',
  '',
];
for (const [name, variable] of settings) {
  const value = process.env[name];
  if (value === undefined) {
    console.error(`${name} must be set.`);
    process.exit(1);
  }
  if (value === '' && name !== 'WIFI_PASSWORD') {
    console.error(`${name} must not be empty.`);
    process.exit(1);
  }
  if (/[\u0000-\u001f\u007f]/.test(value)) {
    console.error(`${name} must not contain control characters.`);
    process.exit(1);
  }
  lines.push(`constexpr char ${variable}[] = ${JSON.stringify(value)};`);
}

try {
  // Restrict an existing header before replacing any of its credentials.
  if (existsSync(args[0])) chmodSync(args[0], 0o600);
  writeFileSync(args[0], `${lines.join('\n')}\n`, { mode: 0o600 });
} catch {
  console.error('Could not write MQTT configuration.');
  process.exit(1);
}
