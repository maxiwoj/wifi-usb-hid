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

        // Get all jiggler parameters
        const type = document.getElementById('jigglerType').value;
        const diameter = document.getElementById('jigglerDiameter').value;
        const delay = document.getElementById('jigglerDelay').value;

        // Send enable command with all parameters
        fetch('/api/jiggler?enable=1&type=' + encodeURIComponent(type) +
              '&diameter=' + encodeURIComponent(diameter) +
              '&delay=' + encodeURIComponent(delay))
          .then(response => response.json())
          .then(data => {
            log('Mouse jiggler enabled (type: ' + type + ', diameter: ' + diameter + 'px, delay: ' + delay + 'ms)');
          })
          .catch(error => {
            log('Error enabling jiggler: ' + error);
          });
      } else {
        toggle.classList.remove('active');
        status.textContent = 'Disabled';

        // Send disable command
        fetch('/api/jiggler?enable=0')
          .then(response => response.json())
          .then(data => {
            log('Mouse jiggler disabled');
          })
          .catch(error => {
            log('Error disabling jiggler: ' + error);
          });
      }
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

    function runQuickScript(scriptId, script) {
      executeScriptText(script);
      log('Running quick script: ' + scriptId);
    }

    function loadQuickScripts() {
      const os = document.getElementById('osSelect').value;
      const quickScriptsDiv = document.getElementById('quickScripts');

      if (!quickScriptsDiv) return; // Not on main page

      quickScriptsDiv.innerHTML = '<p style="color: #6b7280;">Loading...</p>';

      fetch('/api/quickscripts?os=' + encodeURIComponent(os))
        .then(response => response.json())
        .then(scripts => {
          quickScriptsDiv.innerHTML = '';

          if (scripts.length === 0) {
            quickScriptsDiv.innerHTML = '<p style="color: #6b7280; font-style: italic;">No quick scripts for this OS. Add some in <a href="manage-scripts.html" style="color: #667eea;">Manage Scripts</a>.</p>';
            return;
          }

          scripts.forEach(script => {
            const button = document.createElement('button');
            button.className = 'btn ' + script.class;
            button.setAttribute('onclick', "runQuickScript('" + script.id + "', `" + script.script.replace(/`/g, '\\`') + "`)");
            button.textContent = script.label;
            quickScriptsDiv.appendChild(button);
          });
        })
        .catch(error => {
          console.error('Error loading quick scripts:', error);
          quickScriptsDiv.innerHTML = '<p style="color: #ef4444;">Error loading quick scripts. Please refresh the page.</p>';
        });
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
      quickActionsDiv.innerHTML = '<p style="color: #6b7280;">Loading...</p>';

      // Load all actions for this OS from storage
      fetch('/api/quickactions?os=' + encodeURIComponent(os))
        .then(response => response.json())
        .then(actions => {
          quickActionsDiv.innerHTML = '';

          if (actions.length === 0) {
            quickActionsDiv.innerHTML = '<p style="color: #6b7280; font-style: italic;">No quick actions for this OS. Add some in <a href="manage-actions.html" style="color: #667eea;">Manage Actions</a>.</p>';
            return;
          }

          actions.forEach(action => {
            const button = document.createElement('button');
            button.className = 'btn ' + action.class;
            button.setAttribute('onclick', "sendCommand('" + action.cmd + "')");
            button.setAttribute('title', action.desc);
            button.textContent = action.label;
            quickActionsDiv.appendChild(button);
          });
        })
        .catch(error => {
          console.error('Error loading quick actions:', error);
          quickActionsDiv.innerHTML = '<p style="color: #ef4444;">Error loading quick actions. Please refresh the page.</p>';
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

    // Custom OS Management Functions
    function loadCustomOS() {
      fetch('/api/customos')
        .then(response => response.json())
        .then(customOSList => {
          const osSelect = document.getElementById('osSelect');
          const osManagerSelect = document.getElementById('osManagerSelect');

          // Remove any previously added custom OS options from both dropdowns
          [osSelect, osManagerSelect].forEach(select => {
            if (select) {
              const options = select.querySelectorAll('option');
              options.forEach(option => {
                if (option.value !== 'Windows' && option.value !== 'MacOS' && option.value !== 'Linux') {
                  option.remove();
                }
              });

              // Add custom OS to dropdown
              customOSList.forEach(osName => {
                const option = document.createElement('option');
                option.value = osName;
                option.textContent = osName;
                select.appendChild(option);
              });
            }
          });

          // Update the custom OS list display
          displayCustomOSList(customOSList);

          // Restore selected OS after custom OS are loaded
          loadSelectedOS();
        })
        .catch(error => {
          console.error('Error loading custom OS:', error);
        });
    }

    function displayCustomOSList(customOSList) {
      const listDiv = document.getElementById('customOSList');
      if (!listDiv) return;

      // Check if we have a manage page version
      if (typeof displayCustomOSListOnManagePage === 'function') {
        displayCustomOSListOnManagePage(customOSList);
        return;
      }

      if (customOSList.length === 0) {
        listDiv.innerHTML = '<p style="color: #6b7280; font-style: italic;">No custom operating systems.</p>';
        return;
      }

      listDiv.innerHTML = '';
      customOSList.forEach(osName => {
        const item = document.createElement('div');
        item.style.cssText = 'display: flex; justify-content: space-between; align-items: center; padding: 10px; border: 2px solid #e5e7eb; border-radius: 8px; margin-bottom: 10px; background: #f9fafb;';

        const nameSpan = document.createElement('span');
        nameSpan.textContent = osName;
        nameSpan.style.fontWeight = '600';

        const deleteBtn = document.createElement('button');
        deleteBtn.className = 'btn btn-danger';
        deleteBtn.textContent = 'Delete';
        deleteBtn.style.cssText = 'padding: 5px 15px; font-size: 12px;';
        deleteBtn.onclick = function() { deleteCustomOS(osName); };

        item.appendChild(nameSpan);
        item.appendChild(deleteBtn);
        listDiv.appendChild(item);
      });
    }

    function addCustomOS() {
      const osName = prompt('Enter new operating system name:');
      if (!osName || osName.trim() === '') {
        return;
      }

      fetch('/api/customos', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'name=' + encodeURIComponent(osName.trim())
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          log('Custom OS added: ' + osName);
          loadCustomOS();
          loadQuickActionsManager();
        } else {
          log('Error: ' + (data.message || 'Failed to add custom OS'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function deleteCustomOS(osName) {
      if (!confirm('Delete custom OS "' + osName + '" and all its quick actions?')) {
        return;
      }

      fetch('/api/customos/delete', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'name=' + encodeURIComponent(osName)
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          log('Custom OS deleted: ' + osName);

          // Clear saved OS if the deleted OS was selected
          try {
            const savedOS = localStorage.getItem('selectedOS');
            if (savedOS === osName) {
              localStorage.removeItem('selectedOS');
            }
          } catch (error) {
            console.error('Failed to check/clear saved OS:', error);
          }

          loadCustomOS();
          loadQuickActionsManager();
        } else {
          log('Error: ' + (data.message || 'Failed to delete custom OS'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    // Quick Actions Management Functions
    function loadQuickActionsManager() {
      const managerDiv = document.getElementById('quickActionsManager');
      if (!managerDiv) return;

      const os = document.getElementById('osManagerSelect').value;
      managerDiv.innerHTML = '<p style="color: #6b7280;">Loading...</p>';

      fetch('/api/quickactions?os=' + encodeURIComponent(os))
        .then(response => response.json())
        .then(actions => {
          if (actions.length === 0) {
            managerDiv.innerHTML = '<p style="color: #6b7280; font-style: italic;">No custom quick actions for this OS.</p>';
            return;
          }

          managerDiv.innerHTML = '';
          actions.forEach(action => {
            const item = document.createElement('div');
            item.style.cssText = 'display: flex; justify-content: space-between; align-items: center; padding: 10px; border: 2px solid #e5e7eb; border-radius: 8px; margin-bottom: 10px; background: #f9fafb;';

            const infoDiv = document.createElement('div');
            infoDiv.innerHTML = '<strong>' + action.label + '</strong><br><small style="color: #6b7280;">' + action.cmd + ' - ' + action.desc + '</small>';

            const deleteBtn = document.createElement('button');
            deleteBtn.className = 'btn btn-danger';
            deleteBtn.textContent = 'Delete';
            deleteBtn.style.cssText = 'padding: 5px 15px; font-size: 12px;';
            deleteBtn.onclick = function() { deleteQuickAction(os, action.cmd); };

            item.appendChild(infoDiv);
            item.appendChild(deleteBtn);
            managerDiv.appendChild(item);
          });
        })
        .catch(error => {
          managerDiv.innerHTML = '<p style="color: #ef4444;">Error loading quick actions: ' + error + '</p>';
        });
    }

    function saveQuickAction() {
      const os = document.getElementById('osManagerSelect').value;
      const cmd = document.getElementById('quickActionCmd').value.trim();
      const label = document.getElementById('quickActionLabel').value.trim();
      const desc = document.getElementById('quickActionDesc').value.trim();
      const btnClass = document.getElementById('quickActionClass').value;

      if (!cmd || !label) {
        log('Error: Command and label are required');
        return;
      }

      fetch('/api/quickactions', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'os=' + encodeURIComponent(os) +
              '&cmd=' + encodeURIComponent(cmd) +
              '&label=' + encodeURIComponent(label) +
              '&desc=' + encodeURIComponent(desc) +
              '&class=' + encodeURIComponent(btnClass)
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          log('Quick action saved');
          document.getElementById('quickActionCmd').value = '';
          document.getElementById('quickActionLabel').value = '';
          document.getElementById('quickActionDesc').value = '';
          loadQuickActionsManager();
          updateQuickActions();
        } else {
          log('Error: ' + (data.message || 'Failed to save quick action'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function deleteQuickAction(os, cmd) {
      if (!confirm('Delete this quick action?')) {
        return;
      }

      fetch('/api/quickactions/delete', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'os=' + encodeURIComponent(os) + '&cmd=' + encodeURIComponent(cmd)
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          log('Quick action deleted');
          loadQuickActionsManager();
          updateQuickActions();
        } else {
          log('Error: ' + (data.message || 'Failed to delete quick action'));
        }
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function toggleDirectionalControls() {
      const controls = document.getElementById('directionalControls');
      const button = document.getElementById('toggleDirectionalBtn');

      if (controls.style.display === 'none') {
        controls.style.display = 'block';
        button.textContent = '▼ Hide Directional Controls';
      } else {
        controls.style.display = 'none';
        button.textContent = '▶ Show Directional Controls';
      }
    }

    // OS Selection Persistence Functions
    function saveSelectedOS(os) {
      try {
        localStorage.setItem('selectedOS', os);
      } catch (error) {
        console.error('Failed to save selected OS:', error);
      }
    }

    function getOSFromURL() {
      try {
        const params = new URLSearchParams(window.location.search);
        return params.get('os');
      } catch (error) {
        console.error('Failed to get OS from URL:', error);
        return null;
      }
    }

    function loadSelectedOS() {
      try {
        // Check URL parameter first (takes priority)
        let selectedOS = getOSFromURL();

        // If no URL parameter, check localStorage
        if (!selectedOS) {
          selectedOS = localStorage.getItem('selectedOS');
        }

        if (selectedOS) {
          // Set OS in main page selector if it exists
          const osSelect = document.getElementById('osSelect');
          if (osSelect) {
            osSelect.value = selectedOS;

            // Load quick actions and scripts for the selected OS on main page
            updateQuickActions();
            loadQuickScripts();
          }

          // Set OS in manager page selector if it exists
          const osManagerSelect = document.getElementById('osManagerSelect');
          if (osManagerSelect) {
            osManagerSelect.value = selectedOS;

            // Update current OS name display if it exists
            const currentOSName = document.getElementById('currentOSName');
            if (currentOSName) {
              currentOSName.textContent = selectedOS;
            }

            // Load the appropriate manager after OS is set
            if (document.getElementById('quickActionsManager')) {
              loadQuickActionsManager();
            }
            if (typeof loadQuickScriptsManager === 'function') {
              loadQuickScriptsManager();
            }
          }

          // Save to localStorage if it came from URL
          if (getOSFromURL()) {
            saveSelectedOS(selectedOS);
          }
        } else {
          // No saved OS - load defaults for main page if present
          const osSelect = document.getElementById('osSelect');
          if (osSelect) {
            updateQuickActions();
            loadQuickScripts();
          }
        }
      } catch (error) {
        console.error('Failed to load selected OS:', error);
      }
    }

    // Update Quick Actions and Quick Scripts when OS selection changes (main page only)
    const osSelect = document.getElementById('osSelect');
    if (osSelect) {
      osSelect.addEventListener('change', function() {
        saveSelectedOS(this.value);
        updateQuickActions();
        loadQuickScripts();
      });
    }

    // Note: Event listeners for manage pages are set up in their respective inline scripts

    // Initial log and setup
    log('Interface loaded - Ready to use');

    // Load custom OS, then restore selected OS
    // Note: loadSelectedOS() is called inside loadCustomOS() after custom OS are loaded
    // loadSelectedOS() will also trigger loading of quick actions/scripts for the restored OS
    loadCustomOS();

    // Load other content
    loadSavedScripts();
    loadDeviceStatus();

    // Note: Quick actions and scripts are loaded from within loadSelectedOS() after OS is properly set

    // ============================================
    // Keyboard Capture Functionality
    // ============================================

    // Only initialize if keyboard capture modal exists on this page
    if (document.getElementById('keyboardCaptureModal')) {
      initKeyboardCapture();
    }

    function initKeyboardCapture() {
      // Keyboard Capture State
      let captureEnabled = false;
      let pressedKeys = new Set();
      let eventCount = { total: 0, keydown: 0, keyup: 0 };
      let eventHistory = [];
      const MAX_HISTORY = 100;

      // Key mapping for special keys using event.code
      const keyCodeMap = {
        'ControlLeft': 'CTRL',
        'ControlRight': 'CTRL',
        'ShiftLeft': 'SHIFT',
        'ShiftRight': 'SHIFT',
        'AltLeft': 'ALT',
        'AltRight': 'ALT',
        'MetaLeft': 'GUI',
        'MetaRight': 'GUI',
        'Enter': 'ENTER',
        'NumpadEnter': 'ENTER',
        'Escape': 'ESC',
        'Tab': 'TAB',
        'Backspace': 'BACKSPACE',
        'Delete': 'DELETE',
        'ArrowUp': 'UP',
        'ArrowDown': 'DOWN',
        'ArrowLeft': 'LEFT',
        'ArrowRight': 'RIGHT',
        'Home': 'HOME',
        'End': 'END',
        'PageUp': 'PAGEUP',
        'PageDown': 'PAGEDOWN',
        'Insert': 'INSERT',
        'CapsLock': 'CAPSLOCK',
        'Space': 'SPACE'
      };

      // Function keys
      for (let i = 1; i <= 12; i++) {
        keyCodeMap['F' + i] = 'F' + i;
      }

      // Track pressed modifiers
      let pressedModifiers = {
        CTRL: false,
        ALT: false,
        SHIFT: false,
        GUI: false
      };

      // Map event.code to character (for regular keys)
      function codeToChar(code) {
        // Handle letter keys: KeyA -> a
        if (code.startsWith('Key')) {
          return code.substring(3).toLowerCase();
        }
        // Handle digit keys: Digit0 -> 0
        if (code.startsWith('Digit')) {
          return code.substring(5);
        }
        // Handle numpad: Numpad0 -> 0
        if (code.startsWith('Numpad')) {
          const numpadKey = code.substring(6);
          // Numpad operations
          const numpadMap = {
            'Add': '+',
            'Subtract': '-',
            'Multiply': '*',
            'Divide': '/',
            'Decimal': '.'
          };
          return numpadMap[numpadKey] || numpadKey;
        }
        // Handle special character keys
        const specialKeys = {
          'Minus': '-',
          'Equal': '=',
          'BracketLeft': '[',
          'BracketRight': ']',
          'Backslash': '\\',
          'Semicolon': ';',
          'Quote': "'",
          'Comma': ',',
          'Period': '.',
          'Slash': '/',
          'Backquote': '`'
        };
        return specialKeys[code] || null;
      }

      function getKeyCommand(event) {
        const code = event.code;

        // Check if it's a mapped special key
        if (keyCodeMap[code]) {
          return keyCodeMap[code];
        }

        // Try to convert code to character
        const char = codeToChar(code);
        if (char) {
          return char;
        }

        return null;
      }

      function isMobileDevice() {
        // Check multiple indicators of mobile device
        const userAgent = navigator.userAgent || navigator.vendor || window.opera;

        // Check user agent
        const mobileRegex = /android|webos|iphone|ipad|ipod|blackberry|iemobile|opera mini|mobile|tablet/i;
        if (mobileRegex.test(userAgent.toLowerCase())) {
          return true;
        }

        // Check for touch capability (more than 0 touch points)
        if ('maxTouchPoints' in navigator && navigator.maxTouchPoints > 0) {
          return true;
        }

        // Check for touch events
        if ('ontouchstart' in window || navigator.msMaxTouchPoints) {
          return true;
        }

        // Check screen size as fallback (typical mobile width)
        if (window.innerWidth <= 768) {
          return true;
        }

        return false;
      }

      // Make functions available globally
      window.openKeyboardCapture = function() {
        document.getElementById('keyboardCaptureModal').classList.add('active');
        document.body.style.overflow = 'hidden';
      };

      window.closeKeyboardCapture = function() {
        if (captureEnabled) {
          window.toggleKeyboardCapture();
        }
        document.getElementById('keyboardCaptureModal').classList.remove('active');
        document.body.style.overflow = 'auto';
      };

      window.toggleInstructions = function() {
        const content = document.getElementById('instructionsContent');
        const toggle = document.getElementById('instructionsToggle');
        if (content.style.display === 'none') {
          content.style.display = 'block';
          toggle.textContent = '▲';
        } else {
          content.style.display = 'none';
          toggle.textContent = '▼';
        }
      };

      window.toggleMobileControls = function() {
        const mobileInputContainer = document.getElementById('mobileInputContainer');
        const escHint = document.getElementById('escHint');
        const toggleBtn = document.getElementById('mobileControlsToggleBtn');

        if (mobileInputContainer.style.display === 'none') {
          // Show controls
          mobileInputContainer.style.display = 'block';
          escHint.style.display = 'none';
          toggleBtn.textContent = '▼ Hide Mobile Controls';
          // Update GUI button label based on current OS
          window.updateGuiLabel();
        } else {
          // Hide controls
          mobileInputContainer.style.display = 'none';
          escHint.style.display = 'inline';
          toggleBtn.textContent = '▶ Show Mobile Controls';
        }
      };

      window.updateGuiLabel = function() {
        const os = document.getElementById('osSelect')?.value || 'Windows';
        const guiLabel = document.getElementById('guiLabel');

        if (guiLabel) {
          if (os === 'MacOS') {
            guiLabel.textContent = '⌘ Cmd';
          } else if (os === 'Windows') {
            guiLabel.textContent = '⊞ Win';
          } else {
            guiLabel.textContent = 'Super';
          }
        }
      };

      window.toggleModifier = function(key) {
        const btn = document.getElementById(key.toLowerCase() + 'Btn');

        if (pressedModifiers[key]) {
          // Release
          sendCommand('KEY_RELEASE:' + key);
          pressedModifiers[key] = false;
          btn.classList.remove('btn-success');
          btn.classList.add('btn-info');
        } else {
          // Press
          sendCommand('KEY_PRESS:' + key);
          pressedModifiers[key] = true;
          btn.classList.remove('btn-info');
          btn.classList.add('btn-success');
        }
      };

      window.sendHoldKey = function(event, key) {
        event.preventDefault(); // Prevent scrolling on mobile
        sendCommand('KEY_PRESS:' + key);
      };

      window.releaseHoldKey = function(event, key) {
        event.preventDefault(); // Prevent scrolling on mobile
        sendCommand('KEY_RELEASE:' + key);
      };

      window.toggleKeyboardCapture = function() {
        captureEnabled = !captureEnabled;
        const toggle = document.getElementById('captureToggle');
        const status = document.getElementById('captureStatus');
        const indicator = document.getElementById('captureIndicator');
        const mobileInputContainer = document.getElementById('mobileInputContainer');
        const mobileInput = document.getElementById('mobileKeyboardInput');
        const escHint = document.getElementById('escHint');
        const manualMobileButton = document.getElementById('manualMobileButton');

        if (captureEnabled) {
          toggle.classList.add('active');
          status.textContent = 'Active';
          status.style.color = '#10b981';
          indicator.classList.add('active');
          log('Keyboard capture ENABLED');

          // Always show the manual button when capture is enabled
          manualMobileButton.style.display = 'block';
        } else {
          toggle.classList.remove('active');
          status.textContent = 'Disabled';
          status.style.color = '#6b7280';
          indicator.classList.remove('active');
          log('Keyboard capture DISABLED');
          window.releaseAllKeys();

          // Release all pressed modifiers
          for (let key in pressedModifiers) {
            if (pressedModifiers[key]) {
              sendCommand('KEY_RELEASE:' + key);
              pressedModifiers[key] = false;
              const btn = document.getElementById(key.toLowerCase() + 'Btn');
              if (btn) {
                btn.classList.remove('btn-success');
                btn.classList.add('btn-info');
              }
            }
          }

          // Hide mobile input and restore ESC hint
          mobileInput.blur();
          mobileInputContainer.style.display = 'none';
          manualMobileButton.style.display = 'none';
          escHint.style.display = 'inline';

          // Reset mobile controls toggle button text
          const toggleBtn = document.getElementById('mobileControlsToggleBtn');
          if (toggleBtn) {
            toggleBtn.textContent = '▶ Show Mobile Controls';
          }
        }
      };

      function handleKeyboardKeyDown(event) {
        if (!captureEnabled) return;

        const keyCommand = getKeyCommand(event);

        if (!keyCommand) return;

        // ESC key disables capture
        if (event.code === 'Escape') {
          window.toggleKeyboardCapture();
          return;
        }

        // Prevent duplicate events for held keys
        const keyId = '' + keyCommand;
        if (pressedKeys.has(keyId)) return;

        pressedKeys.add(keyId);

        // Prevent default browser behavior
        event.preventDefault();
        event.stopPropagation();

        // Send key press command
        sendCommand('KEY_PRESS:' + keyCommand);

        // Log the event
        const modifiers = [];
        if (event.ctrlKey && keyCommand !== 'CTRL') modifiers.push('Ctrl');
        if (event.shiftKey && keyCommand !== 'SHIFT') modifiers.push('Shift');
        if (event.altKey && keyCommand !== 'ALT') modifiers.push('Alt');
        if (event.metaKey && keyCommand !== 'GUI') modifiers.push('Meta');

        addEventToHistory({
          type: 'keydown',
          key: keyCommand,
          originalKey: event.key !== keyCommand ? event.key : null,
          modifiers: modifiers,
          timestamp: new Date()
        });

        updateKeyboardStats('keydown');
      }

      function handleKeyboardKeyUp(event) {
        if (!captureEnabled) return;

        const keyCommand = getKeyCommand(event);

        if (!keyCommand) return;

        const keyId = '' + keyCommand;
        pressedKeys.delete(keyId);

        // Prevent default browser behavior
        event.preventDefault();
        event.stopPropagation();

        // Send key release command
        sendCommand('KEY_RELEASE:' + keyCommand);

        // Log the event
        const modifiers = [];
        if (event.ctrlKey && keyCommand !== 'CTRL') modifiers.push('Ctrl');
        if (event.shiftKey && keyCommand !== 'SHIFT') modifiers.push('Shift');
        if (event.altKey && keyCommand !== 'ALT') modifiers.push('Alt');
        if (event.metaKey && keyCommand !== 'GUI') modifiers.push('Meta');

        addEventToHistory({
          type: 'keyup',
          key: keyCommand,
          originalKey: event.key !== keyCommand ? event.key : null,
          modifiers: modifiers,
          timestamp: new Date()
        });

        updateKeyboardStats('keyup');
      }

      function addEventToHistory(event) {
        eventHistory.unshift(event);
        if (eventHistory.length > MAX_HISTORY) {
          eventHistory.pop();
        }
        renderKeyboardHistory();
      }

      function renderKeyboardHistory() {
        const historyDiv = document.getElementById('keyHistory');

        if (eventHistory.length === 0) {
          historyDiv.innerHTML = '<p style="color: #6b7280; text-align: center; padding: 20px;">No events captured yet. Enable capture to start.</p>';
          return;
        }

        historyDiv.innerHTML = eventHistory.map(function(event) {
          const time = event.timestamp.toLocaleTimeString();
          const modStr = event.modifiers.length > 0 ? '<span class="key-event-modifiers">(' + event.modifiers.join('+') + ')</span>' : '';

          // Show original key if different (e.g., "ą" when sending "a")
          const keyDisplay = event.originalKey ?
            event.key + ' <span style="color: #9ca3af; font-size: 11px;">(' + event.originalKey + ')</span>' :
            event.key;

          return '<div class="key-event ' + event.type + '">' +
            '<span class="key-event-type">' + (event.type === 'keydown' ? '⬇ DOWN' : '⬆ UP') + '</span>' +
            '<span class="key-event-key">' + keyDisplay + '</span>' +
            modStr +
            '<span style="margin-left: auto; color: #9ca3af; font-size: 11px;">' + time + '</span>' +
            '</div>';
        }).join('');
      }

      function updateKeyboardStats(type) {
        eventCount.total++;
        if (type === 'keydown') eventCount.keydown++;
        if (type === 'keyup') eventCount.keyup++;

        document.getElementById('statTotal').textContent = eventCount.total;
        document.getElementById('statKeyDown').textContent = eventCount.keydown;
        document.getElementById('statKeyUp').textContent = eventCount.keyup;
      }

      window.clearKeyboardHistory = function() {
        if (confirm('Clear event history?')) {
          eventHistory = [];
          eventCount = { total: 0, keydown: 0, keyup: 0 };
          renderKeyboardHistory();
          document.getElementById('statTotal').textContent = '0';
          document.getElementById('statKeyDown').textContent = '0';
          document.getElementById('statKeyUp').textContent = '0';
          log('Keyboard history cleared');
        }
      };

      window.releaseAllKeys = function() {
        sendCommand('KEY_RELEASE_ALL');
        pressedKeys.clear();
        log('All keys released');
      };

      // Add event listeners
      document.addEventListener('keydown', handleKeyboardKeyDown, true);
      document.addEventListener('keyup', handleKeyboardKeyUp, true);

      // Mobile keyboard input handling
      const mobileInput = document.getElementById('mobileKeyboardInput');
      if (mobileInput) {
        mobileInput.addEventListener('input', function(e) {
          // Clear the input to keep it empty
          this.value = '';
        });

        // On mobile, keep input available but don't auto-refocus (iOS won't reopen keyboard anyway)
        mobileInput.addEventListener('blur', function() {
          // Input will lose focus naturally when keyboard closes
          // User can tap again to reopen keyboard if needed
        });
      }

      // Cleanup on page unload
      window.addEventListener('beforeunload', function() {
        if (captureEnabled) {
          window.releaseAllKeys();
        }
      });

      // Release all keys when window loses focus (except on mobile when input is focused)
      window.addEventListener('blur', function() {
        if (captureEnabled && pressedKeys.size > 0 && !isMobileDevice()) {
          window.releaseAllKeys();
        }
      });

      // Close modal when clicking outside
      document.getElementById('keyboardCaptureModal').addEventListener('click', function(e) {
        if (e.target === this) {
          window.closeKeyboardCapture();
        }
      });

      // Update GUI button label when OS selection changes
      const osSelectForGui = document.getElementById('osSelect');
      if (osSelectForGui) {
        osSelectForGui.addEventListener('change', window.updateGuiLabel);
      }
    }
