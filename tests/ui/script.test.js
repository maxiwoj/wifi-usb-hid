/**
 * @jest-environment jsdom
 */

// Mock global fetch
global.fetch = jest.fn();

// Setup DOM elements needed by the script
beforeEach(() => {
  document.body.innerHTML = `
    <div id="activityLog"></div>
    <div id="jigglerToggle"></div>
    <div id="jigglerStatus">Disabled</div>
    <input id="typeText" value="" />
    <select id="osSelect">
      <option value="Windows">Windows</option>
      <option value="MacOS">MacOS</option>
      <option value="Linux">Linux</option>
    </select>
    <div id="quickScripts"></div>
    <div id="quickActions"></div>
    <textarea id="duckyScript"></textarea>
    <input id="scriptName" value="" />
    <div id="deviceIP">Unknown</div>
    <div id="wifiMode">Unknown</div>
    <div id="networkSSID">Unknown</div>
    <div id="statusBadge">Unknown</div>
    <div id="sensitivity">2.0</div>
    <div id="sensitivityValue">2.0x</div>
  `;

  // Reset fetch mock
  fetch.mockClear();

  // Mock successful fetch responses
  fetch.mockResolvedValue({
    json: async () => ({ status: 'ok' })
  });
});

// Test log function
describe('log function', () => {
  // Define log function from script.js
  function log(message) {
    const logDiv = document.getElementById('activityLog');
    const time = new Date().toLocaleTimeString();
    const entry = document.createElement('div');
    entry.className = 'log-entry';
    entry.innerHTML = '<span class="log-time">' + time + '</span>' + message;
    logDiv.insertBefore(entry, logDiv.firstChild);

    // Keep only last 50 entries
    while (logDiv.children.length > 50) {
      logDiv.removeChild(logDiv.lastChild);
    }
  }

  test('should add entry to activity log', () => {
    const logDiv = document.getElementById('activityLog');
    log('Test message');

    expect(logDiv.children.length).toBe(1);
    expect(logDiv.children[0].className).toBe('log-entry');
    expect(logDiv.children[0].innerHTML).toContain('Test message');
  });

  test('should add entries at the beginning (newest first)', () => {
    const logDiv = document.getElementById('activityLog');
    log('First message');
    log('Second message');

    expect(logDiv.children.length).toBe(2);
    expect(logDiv.children[0].innerHTML).toContain('Second message');
    expect(logDiv.children[1].innerHTML).toContain('First message');
  });

  test('should limit log to 50 entries', () => {
    const logDiv = document.getElementById('activityLog');

    // Add 55 entries
    for (let i = 0; i < 55; i++) {
      log(`Message ${i}`);
    }

    expect(logDiv.children.length).toBe(50);
    // Newest message should be first
    expect(logDiv.children[0].innerHTML).toContain('Message 54');
    // Oldest remaining message should be at the end
    expect(logDiv.children[49].innerHTML).toContain('Message 5');
  });
});

// Test sendCommand function
describe('sendCommand function', () => {
  function sendCommand(cmd) {
    return fetch('/api/command', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: 'cmd=' + encodeURIComponent(cmd)
    });
  }

  test('should send POST request to /api/command', async () => {
    await sendCommand('TEST_COMMAND');

    expect(fetch).toHaveBeenCalledWith('/api/command', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: 'cmd=TEST_COMMAND'
    });
  });

  test('should URL encode command parameter', async () => {
    await sendCommand('TEST COMMAND:with/special?chars&more=stuff');

    expect(fetch).toHaveBeenCalledWith('/api/command',
      expect.objectContaining({
        body: expect.stringContaining(encodeURIComponent('TEST COMMAND:with/special?chars&more=stuff'))
      })
    );
  });
});

// Test toggleJiggler function
describe('toggleJiggler function', () => {
  let jigglerEnabled = false;

  function toggleJiggler() {
    jigglerEnabled = !jigglerEnabled;
    const toggle = document.getElementById('jigglerToggle');
    const status = document.getElementById('jigglerStatus');

    if (jigglerEnabled) {
      toggle.classList.add('active');
      status.textContent = 'Enabled';
    } else {
      toggle.classList.remove('active');
      status.textContent = 'Disabled';
    }

    return fetch('/api/jiggler?enable=' + (jigglerEnabled ? '1' : '0'));
  }

  beforeEach(() => {
    jigglerEnabled = false;
  });

  test('should toggle jiggler state from disabled to enabled', async () => {
    await toggleJiggler();

    expect(jigglerEnabled).toBe(true);
    expect(document.getElementById('jigglerStatus').textContent).toBe('Enabled');
    expect(document.getElementById('jigglerToggle').classList.contains('active')).toBe(true);
  });

  test('should toggle jiggler state from enabled to disabled', async () => {
    jigglerEnabled = true;
    document.getElementById('jigglerToggle').classList.add('active');

    await toggleJiggler();

    expect(jigglerEnabled).toBe(false);
    expect(document.getElementById('jigglerStatus').textContent).toBe('Disabled');
    expect(document.getElementById('jigglerToggle').classList.contains('active')).toBe(false);
  });

  test('should send correct API request when enabling', async () => {
    await toggleJiggler();

    expect(fetch).toHaveBeenCalledWith('/api/jiggler?enable=1');
  });

  test('should send correct API request when disabling', async () => {
    jigglerEnabled = true;
    await toggleJiggler();

    expect(fetch).toHaveBeenCalledWith('/api/jiggler?enable=0');
  });
});

// Test typeText functions
describe('typeText functions', () => {
  function typeText() {
    const text = document.getElementById('typeText').value;
    if (text) {
      fetch('/api/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'cmd=' + encodeURIComponent('TYPE:' + text)
      });
      document.getElementById('typeText').value = '';
    }
  }

  function typeTextEnter() {
    const text = document.getElementById('typeText').value;
    if (text) {
      fetch('/api/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'cmd=' + encodeURIComponent('TYPELN:' + text)
      });
      document.getElementById('typeText').value = '';
    }
  }

  test('typeText should send TYPE command', () => {
    document.getElementById('typeText').value = 'Hello World';
    typeText();

    expect(fetch).toHaveBeenCalledWith('/api/command',
      expect.objectContaining({
        body: expect.stringContaining('TYPE%3AHello%20World')
      })
    );
    expect(document.getElementById('typeText').value).toBe('');
  });

  test('typeTextEnter should send TYPELN command', () => {
    document.getElementById('typeText').value = 'Hello World';
    typeTextEnter();

    expect(fetch).toHaveBeenCalledWith('/api/command',
      expect.objectContaining({
        body: expect.stringContaining('TYPELN%3AHello%20World')
      })
    );
    expect(document.getElementById('typeText').value).toBe('');
  });

  test('should not send command if input is empty', () => {
    document.getElementById('typeText').value = '';
    typeText();

    expect(fetch).not.toHaveBeenCalled();
  });
});

// Test mouse movement calculations
describe('mouse movement calculations', () => {
  function calculateMouseDelta(lastX, lastY, currentX, currentY, sensitivity) {
    const deltaX = Math.round((currentX - lastX) * sensitivity);
    const deltaY = Math.round((currentY - lastY) * sensitivity);
    return { deltaX, deltaY };
  }

  test('should calculate delta correctly with sensitivity 1.0', () => {
    const result = calculateMouseDelta(100, 100, 110, 115, 1.0);
    expect(result.deltaX).toBe(10);
    expect(result.deltaY).toBe(15);
  });

  test('should calculate delta correctly with sensitivity 2.0', () => {
    const result = calculateMouseDelta(100, 100, 110, 115, 2.0);
    expect(result.deltaX).toBe(20);
    expect(result.deltaY).toBe(30);
  });

  test('should calculate delta correctly with sensitivity 0.5', () => {
    const result = calculateMouseDelta(100, 100, 110, 115, 0.5);
    expect(result.deltaX).toBe(5);
    expect(result.deltaY).toBe(8); // 7.5 rounds to 8
  });

  test('should handle negative movement', () => {
    const result = calculateMouseDelta(100, 100, 90, 85, 1.0);
    expect(result.deltaX).toBe(-10);
    expect(result.deltaY).toBe(-15);
  });

  test('should handle zero movement', () => {
    const result = calculateMouseDelta(100, 100, 100, 100, 2.0);
    expect(result.deltaX).toBe(0);
    expect(result.deltaY).toBe(0);
  });

  test('should round fractional values', () => {
    const result = calculateMouseDelta(100, 100, 103, 107, 1.5);
    expect(result.deltaX).toBe(5); // 4.5 rounds to 5
    expect(result.deltaY).toBe(11); // 10.5 rounds to 11
  });
});

// Test command formatting
describe('command formatting', () => {
  function formatMouseMoveCommand(deltaX, deltaY) {
    return 'MOUSE_MOVE:' + deltaX + ',' + deltaY;
  }

  function formatTypeCommand(text) {
    return 'TYPE:' + text;
  }

  test('should format mouse move command correctly', () => {
    expect(formatMouseMoveCommand(10, 20)).toBe('MOUSE_MOVE:10,20');
    expect(formatMouseMoveCommand(-5, 15)).toBe('MOUSE_MOVE:-5,15');
    expect(formatMouseMoveCommand(0, 0)).toBe('MOUSE_MOVE:0,0');
  });

  test('should format type command correctly', () => {
    expect(formatTypeCommand('Hello World')).toBe('TYPE:Hello World');
    expect(formatTypeCommand('test@example.com')).toBe('TYPE:test@example.com');
  });
});
