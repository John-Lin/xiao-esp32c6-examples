import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { chmodSync, existsSync, mkdtempSync, rmSync, statSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const generator = fileURLToPath(new URL('../scripts/generate-mqtt-config.mjs', import.meta.url));
const builder = fileURLToPath(new URL('../scripts/build-mqtt-led.sh', import.meta.url));
const fixture = {
  WIFI_SSID: 'Test "Lab" \\ 測試',
  WIFI_PASSWORD: 'test-only $;\'"\\ password',
  MQTT_HOST: '192.0.2.90',
};

function workspace(t) {
  const directory = mkdtempSync(join(tmpdir(), 'mqtt-config-test-'));
  t.after(() => rmSync(directory, { recursive: true, force: true }));
  return directory;
}

function generate(output, values = fixture, args = [output]) {
  const env = { ...process.env };
  for (const name of Object.keys(fixture)) delete env[name];
  return spawnSync(process.execPath, [generator, ...args], {
    env: { ...env, ...values }, encoding: 'utf8',
  });
}

function readCompiledConfig(directory) {
  // Incorrect C++ escaping or assignment must change the actual compiled bytes.
  const source = join(directory, 'read-config.cpp');
  const executable = join(directory, 'read-config');
  writeFileSync(source, `#include <iostream>
#include "Config.h"
int main() {
  std::cout << wifiSsid << '\\0' << wifiPassword << '\\0' << mqttHost;
}
`);
  const compile = spawnSync(process.env.CXX || 'c++', [
    '-std=c++11', '-Wall', '-Wextra', '-Werror', source, '-o', executable,
  ], { encoding: 'utf8' });
  assert.equal(compile.status, 0, compile.stderr);
  assert.equal(compile.stdout, '');
  assert.equal(compile.stderr, '');
  const result = spawnSync(executable);
  assert.equal(result.status, 0);
  assert.equal(result.stderr.length, 0);
  return result.stdout;
}

test('generated header preserves quotes, backslashes, spaces and UTF-8 in compiled C++', t => {
  const directory = workspace(t);
  const result = generate(join(directory, 'Config.h'));
  assert.equal(result.status, 0);
  assert.equal(result.stdout, '');
  assert.equal(result.stderr, '');
  assert.deepEqual(readCompiledConfig(directory), Buffer.from(
    'Test "Lab" \\ 測試\0test-only $;\'"\\ password\x00192.0.2.90',
  ));
});

for (const name of Object.keys(fixture)) {
  test(`missing ${name} fails without creating a header or leaking credentials`, t => {
    const output = join(workspace(t), 'Config.h');
    const values = { ...fixture };
    delete values[name];
    const result = generate(output, values);
    assert.equal(result.status, 1);
    assert.equal(result.stdout, '');
    assert.equal(result.stderr, `${name} must be set.\n`);
    assert.equal(existsSync(output), false);
  });
}

for (const name of ['WIFI_SSID', 'MQTT_HOST']) {
  test(`empty ${name} fails rather than building unusable settings`, t => {
    const output = join(workspace(t), 'Config.h');
    const result = generate(output, { ...fixture, [name]: '' });
    assert.equal(result.status, 1);
    assert.equal(result.stdout, '');
    assert.equal(result.stderr, `${name} must not be empty.\n`);
    assert.equal(existsSync(output), false);
  });
}

test('explicitly empty password supports an open Wi-Fi network', t => {
  const directory = workspace(t);
  const result = generate(join(directory, 'Config.h'), { ...fixture, WIFI_PASSWORD: '' });
  assert.equal(result.status, 0);
  assert.equal(result.stderr, '');
  assert.deepEqual(readCompiledConfig(directory), Buffer.from('Test "Lab" \\ 測試\0\x00192.0.2.90'));
});

for (const value of ['test-secret\nline', 'test-secret\ttab']) {
  test(`control characters fail without leaking the invalid password (${value.includes('\n') ? 'newline' : 'tab'})`, t => {
    const output = join(workspace(t), 'Config.h');
    const result = generate(output, { ...fixture, WIFI_PASSWORD: value });
    assert.equal(result.status, 1);
    assert.equal(result.stdout, '');
    assert.equal(result.stderr, 'WIFI_PASSWORD must not contain control characters.\n');
    assert.equal(existsSync(output), false);
  });
}

test('regeneration uses new environment values and restricts file permissions', t => {
  const directory = workspace(t);
  const output = join(directory, 'Config.h');
  assert.equal(generate(output).status, 0);
  assert.equal(statSync(output).mode & 0o777, 0o600);
  chmodSync(output, 0o644);
  const result = generate(output, {
    WIFI_SSID: 'New network', WIFI_PASSWORD: 'new-test-password', MQTT_HOST: '192.0.2.91',
  });
  assert.equal(result.status, 0);
  assert.equal(result.stderr, '');
  assert.equal(statSync(output).mode & 0o777, 0o600);
  assert.deepEqual(readCompiledConfig(directory), Buffer.from('New network\0new-test-password\x00192.0.2.91'));
});

test('write failure is reported without exposing configuration values', t => {
  const result = generate(join(workspace(t), 'missing-directory', 'Config.h'));
  assert.equal(result.status, 1);
  assert.equal(result.stdout, '');
  assert.equal(result.stderr, 'Could not write MQTT configuration.\n');
});

test('help works without credentials', () => {
  const result = generate('', {}, ['--help']);
  assert.equal(result.status, 0);
  assert.match(result.stdout, /Usage:/);
  assert.match(result.stdout, /WIFI_SSID/);
  assert.equal(result.stderr, '');
});

test('build script help works without loading local credentials', () => {
  const result = spawnSync('bash', [builder, '--help'], { encoding: 'utf8' });
  assert.equal(result.status, 0);
  assert.match(result.stdout, /Usage:/);
  assert.match(result.stdout, /--upload/);
  assert.equal(result.stderr, '');
});

for (const args of [['--upload'], ['--upload', ''], ['unknown'], ['--upload', 'port', 'extra']]) {
  test(`build script rejects invalid arguments: ${JSON.stringify(args)}`, () => {
    const result = spawnSync('bash', [builder, ...args], { encoding: 'utf8' });
    assert.equal(result.status, 2);
    assert.equal(result.stdout, '');
    assert.match(result.stderr, /^Usage:/);
  });
}

test('build script stops before compile/upload when configuration is missing', t => {
  const env = { ...process.env };
  for (const name of Object.keys(fixture)) delete env[name];
  const result = spawnSync('bash', [builder, '--upload', 'not-a-real-port'], {
    cwd: workspace(t), env, encoding: 'utf8',
  });
  assert.equal(result.status, 1);
  assert.equal(result.stdout, '');
  assert.equal(result.stderr, 'WIFI_SSID must be set.\n');
});

for (const args of [[], ['one', 'two']]) {
  test(`wrong argument count (${args.length}) reports usage`, () => {
    const result = generate('', {}, args);
    assert.equal(result.status, 1);
    assert.equal(result.stdout, '');
    assert.match(result.stderr, /^Usage:/);
  });
}
