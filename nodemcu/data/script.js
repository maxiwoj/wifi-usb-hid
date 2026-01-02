let jigglerEnabled = false;

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

    function sendCommand(cmd) {
      fetch('/api/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'cmd=' + encodeURIComponent(cmd)
      })
      .then(response => response.json())
      .then(data => {
        log('Command sent: ' + cmd);
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

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

      fetch('/api/jiggler?enable=' + (jigglerEnabled ? '1' : '0'))
        .then(response => response.json())
        .then(data => {
          log('Mouse jiggler ' + (jigglerEnabled ? 'enabled' : 'disabled'));
        });
    }

    function typeText() {
      const text = document.getElementById('typeText').value;
      if (text) {
        sendCommand('TYPE:' + text);
        document.getElementById('typeText').value = '';
      }
    }

    function typeTextEnter() {
      const text = document.getElementById('typeText').value;
      if (text) {
        sendCommand('TYPELN:' + text);
        document.getElementById('typeText').value = '';
      }
    }

    function runQuickScript(scriptName) {
      const os = document.getElementById('osSelect').value;
      const scripts = {
        'Windows': {
          'editor': 'GUI r\nDELAY 500\nSTRING notepad\nENTER',
          'terminal': 'GUI r\nDELAY 500\nSTRING cmd\nENTER',
          'calculator': 'GUI r\nDELAY 500\nSTRING calc\nENTER',
          'browser': 'GUI r\nDELAY 500\nSTRING chrome\nENTER'
        },
        'MacOS': {
          'editor': 'GUI SPACE\nDELAY 500\nSTRING textedit\nENTER',
          'terminal': 'GUI SPACE\nDELAY 500\nSTRING terminal\nENTER',
          'calculator': 'GUI SPACE\nDELAY 500\nSTRING calculator\nENTER',
          'browser': 'GUI SPACE\nDELAY 500\nSTRING safari\nENTER'
        },
        'Linux': {
          'editor': 'CTRL ALT T\nDELAY 1000\nSTRING gedit\nENTER',
          'terminal': 'CTRL ALT T',
          'calculator': 'GUI\nDELAY 500\nSTRING calc\nENTER',
          'browser': 'GUI\nDELAY 500\nSTRING firefox\nENTER'
        }
      };

      const script = scripts[os][scriptName];
      if (script) {
        executeScriptText(script);
      }
    }

    function executeScript() {
      const script = document.getElementById('duckyScript').value;
      executeScriptText(script);
    }

    function executeScriptText(script) {
      fetch('/api/script', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'script=' + encodeURIComponent(script)
      })
      .then(response => response.json())
      .then(data => {
        log('Script executed');
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function updateQuickActions() {
      const os = document.getElementById('osSelect').value;
      const quickActionsDiv = document.getElementById('quickActions');
      quickActionsDiv.innerHTML = '';

      const actions = {
        'Windows': [
          { cmd: 'GUI_R', label: 'Win+R', desc: 'Run Dialog', class: 'btn-primary' },
          { cmd: 'GUI_SPACE', label: 'Win+Space', desc: 'Input Switch', class: 'btn-primary' },
          { cmd: 'GUI_D', label: 'Win+D', desc: 'Show Desktop', class: 'btn-primary' },
          { cmd: 'ALT_TAB', label: 'Alt+Tab', desc: 'Switch Apps', class: 'btn-primary' },
          { cmd: 'ENTER', label: 'Enter', desc: 'Enter', class: 'btn-success' },
          { cmd: 'ESC', label: 'Escape', desc: 'Escape', class: 'btn-warning' },
          { cmd: 'TAB', label: 'Tab', desc: 'Tab', class: 'btn-info' }
        ],
        'MacOS': [
          { cmd: 'GUI_SPACE', label: '⌘+Space', desc: 'Spotlight', class: 'btn-primary' },
          { cmd: 'GUI_TAB', label: '⌘+Tab', desc: 'Switch Apps', class: 'btn-primary' },
          { cmd: 'GUI_D', label: '⌘+D', desc: 'Show Desktop', class: 'btn-primary' },
          { cmd: 'GUI_H', label: '⌘+H', desc: 'Hide App', class: 'btn-primary' },
          { cmd: 'GUI_W', label: '⌘+W', desc: 'Close Window', class: 'btn-primary' },
          { cmd: 'ENTER', label: 'Enter', desc: 'Enter', class: 'btn-success' },
          { cmd: 'ESC', label: 'Escape', desc: 'Escape', class: 'btn-warning' },
          { cmd: 'TAB', label: 'Tab', desc: 'Tab', class: 'btn-info' }
        ],
        'Linux': [
          { cmd: 'GUI', label: 'Super', desc: 'Super Key', class: 'btn-primary' },
          { cmd: 'GUI_SPACE', label: 'Super+Space', desc: 'App Launcher', class: 'btn-primary' },
          { cmd: 'ALT_TAB', label: 'Alt+Tab', desc: 'Switch Apps', class: 'btn-primary' },
          { cmd: 'CTRL_ALT_T', label: 'Ctrl+Alt+T', desc: 'Terminal', class: 'btn-primary' },
          { cmd: 'ENTER', label: 'Enter', desc: 'Enter', class: 'btn-success' },
          { cmd: 'ESC', label: 'Escape', desc: 'Escape', class: 'btn-warning' },
          { cmd: 'TAB', label: 'Tab', desc: 'Tab', class: 'btn-info' }
        ]
      };

      const osActions = actions[os] || actions['Windows'];
      osActions.forEach(action => {
        const button = document.createElement('button');
        button.className = 'btn ' + action.class;
        button.setAttribute('onclick', "sendCommand('" + action.cmd + "')");
        button.setAttribute('title', action.desc);
        button.textContent = action.label;
        quickActionsDiv.appendChild(button);
      });
    }

    // Script Management Functions
    function saveScript() {
      const script = document.getElementById('duckyScript').value;
      const name = document.getElementById('scriptName').value.trim();

      if (!script) {
        log('Error: Script is empty');
        return;
      }

      if (!name) {
        log('Error: Please enter a script name');
        return;
      }

      fetch('/api/scripts', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'name=' + encodeURIComponent(name) + '&script=' + encodeURIComponent(script)
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          log('Script saved: ' + name);
          document.getElementById('scriptName').value = '';
          loadSavedScripts();
        } else {
          log('Error saving script: ' + (data.message || 'Unknown error'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function loadSavedScripts() {
      const listDiv = document.getElementById('savedScriptsList');
      listDiv.innerHTML = '<p style="color: #6b7280;">Loading...</p>';

      fetch('/api/scripts')
        .then(response => response.json())
        .then(scripts => {
          if (scripts.length === 0) {
            listDiv.innerHTML = '<p style="color: #6b7280; font-style: italic;">No saved scripts.</p>';
            return;
          }

          listDiv.innerHTML = '';
          scripts.forEach(script => {
            const item = document.createElement('div');
            item.style.cssText = 'display: flex; justify-content: space-between; align-items: center; padding: 10px; border: 2px solid #e5e7eb; border-radius: 8px; margin-bottom: 10px; background: #f9fafb;';

            const nameSpan = document.createElement('span');
            nameSpan.textContent = script.name;
            nameSpan.style.fontWeight = '600';

            const buttonGroup = document.createElement('div');
            buttonGroup.style.cssText = 'display: flex; gap: 5px;';

            const loadBtn = document.createElement('button');
            loadBtn.className = 'btn btn-primary';
            loadBtn.textContent = 'Load';
            loadBtn.style.cssText = 'padding: 5px 15px; font-size: 12px;';
            loadBtn.onclick = function() { loadScriptToEditor(script.name); };

            const deleteBtn = document.createElement('button');
            deleteBtn.className = 'btn btn-danger';
            deleteBtn.textContent = 'Delete';
            deleteBtn.style.cssText = 'padding: 5px 15px; font-size: 12px;';
            deleteBtn.onclick = function() { deleteSavedScript(script.name); };

            buttonGroup.appendChild(loadBtn);
            buttonGroup.appendChild(deleteBtn);

            item.appendChild(nameSpan);
            item.appendChild(buttonGroup);
            listDiv.appendChild(item);
          });
        })
        .catch(error => {
          listDiv.innerHTML = '<p style="color: #ef4444;">Error loading scripts: ' + error + '</p>';
        });
    }

    function loadScriptToEditor(name) {
      fetch('/api/scripts/load', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'name=' + encodeURIComponent(name)
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          document.getElementById('duckyScript').value = data.script.replace(/\\n/g, '\n');
          document.getElementById('scriptName').value = data.name;
          log('Script loaded: ' + data.name);
        } else {
          log('Error loading script: ' + (data.message || 'Unknown error'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function deleteSavedScript(name) {
      if (!confirm('Are you sure you want to delete this script?')) {
        return;
      }

      fetch('/api/scripts/delete', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'name=' + encodeURIComponent(name)
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          log('Script deleted');
          loadSavedScripts();
        } else {
          log('Error deleting script: ' + (data.message || 'Unknown error'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    // Load and display device status
    function loadDeviceStatus() {
      fetch('/api/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('deviceIP').textContent = data.ip || 'Unknown';
          document.getElementById('wifiMode').textContent = data.wifi_mode || 'Unknown';
          document.getElementById('networkSSID').textContent = data.ssid || 'Unknown';

          // Update status badge
          if (data.connected) {
            document.getElementById('statusBadge').textContent = 'Connected';
            document.getElementById('statusBadge').style.background = '#10b981';
          } else {
            document.getElementById('statusBadge').textContent = 'AP Mode';
            document.getElementById('statusBadge').style.background = '#f59e0b';
          }
        })
        .catch(error => {
          log('Error loading device status: ' + error);
          document.getElementById('deviceIP').textContent = 'Error';
          document.getElementById('wifiMode').textContent = 'Error';
          document.getElementById('networkSSID').textContent = 'Error';
        });
    }

    // Trackpad functionality
    (function() {
      const trackpad = document.getElementById('trackpad');
      const cursor = document.getElementById('trackpadCursor');
      const sensitivitySlider = document.getElementById('sensitivity');
      const sensitivityValue = document.getElementById('sensitivityValue');

      let isDragging = false;
      let lastX = 0;
      let lastY = 0;
      let sensitivity = parseFloat(sensitivitySlider.value);
      let clickStartTime = 0;
      let hasMoved = false;
      let lastClickTime = 0;
      let isButtonHeld = false;
      let usedDoubleClickForHold = false;
      const DOUBLE_CLICK_THRESHOLD = 400; // ms

      // Update sensitivity display
      function updateSensitivityDisplay() {
        sensitivity = parseFloat(sensitivitySlider.value);
        sensitivityValue.textContent = sensitivity.toFixed(1) + 'x';
      }

      sensitivitySlider.addEventListener('input', updateSensitivityDisplay);

      // Helper function to check if point is inside trackpad
      function isInsideTrackpad(clientX, clientY) {
        const rect = trackpad.getBoundingClientRect();
        return clientX >= rect.left && clientX <= rect.right &&
               clientY >= rect.top && clientY <= rect.bottom;
      }

      // Handle mouse movement (for both mouse and touch)
      function handleMove(clientX, clientY) {
        const deltaX = Math.round((clientX - lastX) * sensitivity);
        const deltaY = Math.round((clientY - lastY) * sensitivity);

        if (deltaX !== 0 || deltaY !== 0) {
          sendCommand('MOUSE_MOVE:' + deltaX + ',' + deltaY);
          lastX = clientX;
          lastY = clientY;
          hasMoved = true;
        }

        // Update cursor position
        const rect = trackpad.getBoundingClientRect();
        const x = Math.max(0, Math.min(rect.width, clientX - rect.left));
        const y = Math.max(0, Math.min(rect.height, clientY - rect.top));
        cursor.style.left = x + 'px';
        cursor.style.top = y + 'px';
      }

      // Mouse events
      trackpad.addEventListener('mousedown', function(e) {
        const currentTime = Date.now();
        const timeSinceLastClick = currentTime - lastClickTime;

        isDragging = true;
        hasMoved = false;
        clickStartTime = currentTime;
        lastX = e.clientX;
        lastY = e.clientY;

        // Check if this is a double-click-and-hold gesture
        if (timeSinceLastClick < DOUBLE_CLICK_THRESHOLD && !isButtonHeld) {
          // Second click within double-click time - hold the button
          isButtonHeld = true;
          usedDoubleClickForHold = true;
          sendCommand('MOUSE_PRESS');
          log('Mouse button held (drag to select)');
        }

        const rect = trackpad.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        cursor.style.left = x + 'px';
        cursor.style.top = y + 'px';
        cursor.style.display = 'block';

        e.preventDefault();
      });

      trackpad.addEventListener('mousemove', function(e) {
        if (isDragging) {
          handleMove(e.clientX, e.clientY);
          e.preventDefault();
        }
      });

      trackpad.addEventListener('mouseup', function(e) {
        if (isDragging) {
          const clickDuration = Date.now() - clickStartTime;

          // Release held button if active
          if (isButtonHeld) {
            sendCommand('MOUSE_RELEASE');
            isButtonHeld = false;
            log('Mouse button released');
            // Clear the flag after a short delay to prevent dblclick event
            setTimeout(function() {
              usedDoubleClickForHold = false;
            }, 100);
          }
          // If it was a quick click without much movement, treat as click
          else if (!hasMoved && clickDuration < 200) {
            sendCommand('MOUSE_LEFT');
            lastClickTime = Date.now();
          }

          isDragging = false;
          cursor.style.display = 'none';
          e.preventDefault();
        }
      });

      trackpad.addEventListener('mouseleave', function(e) {
        if (isDragging) {
          // Release button if held when leaving trackpad
          if (isButtonHeld) {
            sendCommand('MOUSE_RELEASE');
            isButtonHeld = false;
            usedDoubleClickForHold = false;
            log('Mouse button released (left trackpad)');
          }
          isDragging = false;
          cursor.style.display = 'none';
        }
      });

      // Handle double-click on trackpad
      trackpad.addEventListener('dblclick', function(e) {
        // Only send double-click if we didn't use it for button hold
        if (!usedDoubleClickForHold) {
          sendCommand('MOUSE_DOUBLE');
        }
        e.preventDefault();
      });

      // Touch events for mobile/tablet support
      trackpad.addEventListener('touchstart', function(e) {
        if (e.touches.length === 1) {
          const currentTime = Date.now();
          const timeSinceLastClick = currentTime - lastClickTime;

          isDragging = true;
          hasMoved = false;
          clickStartTime = currentTime;
          const touch = e.touches[0];
          lastX = touch.clientX;
          lastY = touch.clientY;

          // Check if this is a double-tap-and-hold gesture
          if (timeSinceLastClick < DOUBLE_CLICK_THRESHOLD && !isButtonHeld) {
            // Second tap within double-tap time - hold the button
            isButtonHeld = true;
            usedDoubleClickForHold = true;
            sendCommand('MOUSE_PRESS');
            log('Mouse button held (drag to select)');
          }

          const rect = trackpad.getBoundingClientRect();
          const x = touch.clientX - rect.left;
          const y = touch.clientY - rect.top;
          cursor.style.left = x + 'px';
          cursor.style.top = y + 'px';
          cursor.style.display = 'block';

          e.preventDefault();
        }
      });

      trackpad.addEventListener('touchmove', function(e) {
        if (isDragging && e.touches.length === 1) {
          const touch = e.touches[0];
          handleMove(touch.clientX, touch.clientY);
          e.preventDefault();
        }
      });

      trackpad.addEventListener('touchend', function(e) {
        if (isDragging) {
          const clickDuration = Date.now() - clickStartTime;

          // Release held button if active
          if (isButtonHeld) {
            sendCommand('MOUSE_RELEASE');
            isButtonHeld = false;
            usedDoubleClickForHold = false;
            log('Mouse button released');
          }
          // If it was a quick tap without much movement, treat as click
          else if (!hasMoved && clickDuration < 200) {
            sendCommand('MOUSE_LEFT');
            lastClickTime = Date.now();
          }

          isDragging = false;
          cursor.style.display = 'none';
          e.preventDefault();
        }
      });

      trackpad.addEventListener('touchcancel', function(e) {
        if (isDragging) {
          // Release button if held when touch is cancelled
          if (isButtonHeld) {
            sendCommand('MOUSE_RELEASE');
            isButtonHeld = false;
            usedDoubleClickForHold = false;
            log('Mouse button released (touch cancelled)');
          }
          isDragging = false;
          cursor.style.display = 'none';
          e.preventDefault();
        }
      });

      // Initialize sensitivity display
      updateSensitivityDisplay();
    })();

    // Update Quick Actions when OS selection changes
    document.getElementById('osSelect').addEventListener('change', updateQuickActions);

    // Initial log and setup
    log('Interface loaded - Ready to use');
    updateQuickActions();
    loadSavedScripts();
    loadDeviceStatus();
