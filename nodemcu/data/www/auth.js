// WiFi USB HID - Auth module
// Token-based authentication with localStorage persistence.
// Replaces HTTP Basic Auth so mobile browsers don't re-prompt on every visit.

(function () {
  'use strict';

  var TOKEN_KEY = 'wifiHidToken';

  function getToken() {
    return localStorage.getItem(TOKEN_KEY);
  }

  function setToken(token) {
    localStorage.setItem(TOKEN_KEY, token);
  }

  function clearToken() {
    localStorage.removeItem(TOKEN_KEY);
  }

  // Auth-aware fetch wrapper – adds Bearer token to every API request.
  // Use this instead of bare fetch() for all /api/* calls.
  window.apiFetch = function (url, options) {
    options = options || {};
    var token = getToken();
    if (token) {
      options.headers = options.headers || {};
      options.headers['Authorization'] = 'Bearer ' + token;
    }
    return fetch(url, options).then(function (response) {
      if (response.status === 401) {
        clearToken();
        showLoginOverlay();
        return Promise.reject(new Error('Authentication required'));
      }
      return response;
    });
  };

  function doLogin(username, password) {
    return fetch('/api/login', {
      method: 'POST',
      headers: {'Content-Type': 'application/x-www-form-urlencoded'},
      body: 'username=' + encodeURIComponent(username) + '&password=' + encodeURIComponent(password)
    }).then(function (response) {
      if (!response.ok) {
        return response.json().then(function (data) {
          throw new Error(data.message || 'Login failed');
        });
      }
      return response.json();
    }).then(function (data) {
      setToken(data.token);
      return data;
    });
  }

  function doLogout() {
    var token = getToken();
    if (token) {
      fetch('/api/logout', {
        method: 'POST',
        headers: {'Authorization': 'Bearer ' + token}
      }).catch(function () {});
    }
    clearToken();
    showLoginOverlay();
  }

  window.authLogout = doLogout;

  function showLoginOverlay() {
    var existing = document.getElementById('authOverlay');
    if (existing) {
      existing.style.display = 'flex';
      var passField = existing.querySelector('#authPass');
      if (passField) { passField.value = ''; passField.focus(); }
      return;
    }

    var overlay = document.createElement('div');
    overlay.id = 'authOverlay';
    overlay.style.cssText = [
      'position:fixed', 'top:0', 'left:0', 'width:100%', 'height:100%',
      'background:linear-gradient(135deg,#667eea 0%,#764ba2 100%)',
      'display:flex', 'align-items:center', 'justify-content:center',
      'z-index:9999',
      'font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif'
    ].join(';');

    overlay.innerHTML =
      '<div style="background:white;border-radius:15px;padding:40px;width:90%;max-width:380px;box-shadow:0 20px 60px rgba(0,0,0,0.3);">' +
        '<h2 style="margin:0 0 6px;color:#1f2937;font-size:22px;text-align:center;">WiFi USB HID</h2>' +
        '<p style="margin:0 0 24px;color:#6b7280;font-size:14px;text-align:center;">Sign in to continue</p>' +
        '<div id="authError" style="display:none;background:#fee2e2;border:1px solid #fca5a5;color:#dc2626;padding:10px 14px;border-radius:8px;font-size:14px;margin-bottom:16px;"></div>' +
        '<div style="margin-bottom:14px;">' +
          '<label style="display:block;font-size:13px;font-weight:600;color:#374151;margin-bottom:5px;">Username</label>' +
          '<input id="authUser" type="text" autocomplete="username" value="admin"' +
            ' style="width:100%;padding:10px 14px;border:2px solid #e5e7eb;border-radius:8px;font-size:16px;box-sizing:border-box;outline:none;">' +
        '</div>' +
        '<div style="margin-bottom:24px;">' +
          '<label style="display:block;font-size:13px;font-weight:600;color:#374151;margin-bottom:5px;">Password</label>' +
          '<input id="authPass" type="password" autocomplete="current-password"' +
            ' style="width:100%;padding:10px 14px;border:2px solid #e5e7eb;border-radius:8px;font-size:16px;box-sizing:border-box;outline:none;">' +
        '</div>' +
        '<button id="authBtn" style="width:100%;padding:12px;background:linear-gradient(135deg,#667eea,#764ba2);color:white;border:none;border-radius:8px;font-size:16px;font-weight:600;cursor:pointer;" onclick="window._doAuthLogin()">Sign In</button>' +
      '</div>';

    document.body.appendChild(overlay);

    overlay.querySelector('#authUser').addEventListener('keydown', function (e) {
      if (e.key === 'Enter') overlay.querySelector('#authPass').focus();
    });
    overlay.querySelector('#authPass').addEventListener('keydown', function (e) {
      if (e.key === 'Enter') window._doAuthLogin();
    });

    // Auto-focus password (username is pre-filled with "admin")
    setTimeout(function () { overlay.querySelector('#authPass').focus(); }, 50);
  }

  function hideLoginOverlay() {
    var overlay = document.getElementById('authOverlay');
    if (overlay) overlay.style.display = 'none';
  }

  window._doAuthLogin = function () {
    var username = document.getElementById('authUser').value;
    var password = document.getElementById('authPass').value;
    var btn = document.getElementById('authBtn');
    var errorDiv = document.getElementById('authError');

    btn.disabled = true;
    btn.textContent = 'Signing in\u2026';
    errorDiv.style.display = 'none';

    doLogin(username, password)
      .then(function () {
        hideLoginOverlay();
        if (typeof window.onAuthSuccess === 'function') {
          window.onAuthSuccess();
        } else {
          window.location.reload();
        }
      })
      .catch(function (err) {
        btn.disabled = false;
        btn.textContent = 'Sign In';
        errorDiv.textContent = err.message || 'Invalid credentials';
        errorDiv.style.display = 'block';
        document.getElementById('authPass').value = '';
        document.getElementById('authPass').focus();
      });
  };

  // On every page load: if no token is stored, show the login overlay immediately.
  document.addEventListener('DOMContentLoaded', function () {
    if (!getToken()) {
      showLoginOverlay();
    }
  });
})();
