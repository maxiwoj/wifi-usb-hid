#!/usr/bin/env python3
"""
Remote Type CLI - Interactive terminal for WiFi USB HID device.

Reads input and sends TYPE/TYPELN commands to the device via REST API.
Supports line-by-line mode (default) and raw keystroke mode (-k).
"""

import argparse
import base64
import json
import sys
import urllib.request
import urllib.error
import urllib.parse
import ssl


def build_auth_header(user, password):
    credentials = base64.b64encode(f"{user}:{password}".encode()).decode()
    return {"Authorization": f"Basic {credentials}"}


def send_command(base_url, headers, cmd, ssl_ctx):
    data = urllib.parse.urlencode({"cmd": cmd}).encode()
    req = urllib.request.Request(
        f"{base_url}/api/command", data=data, headers=headers, method="POST"
    )
    try:
        with urllib.request.urlopen(req, context=ssl_ctx, timeout=5) as resp:
            body = json.loads(resp.read().decode())
            return body.get("status") == "ok", body.get("message", "")
    except urllib.error.HTTPError as e:
        if e.code == 401:
            print("Error: Authentication failed (401). Check username/password.", file=sys.stderr)
            sys.exit(1)
        return False, f"HTTP {e.code}: {e.reason}"
    except urllib.error.URLError as e:
        return False, f"Connection error: {e.reason}"
    except Exception as e:
        return False, str(e)


def build_type_cmd(text, delay, newline):
    if delay > 0:
        prefix = "TYPELN_DELAY" if newline else "TYPE_DELAY"
        return f"{prefix}:{delay}:{text}"
    else:
        prefix = "TYPELN" if newline else "TYPE"
        return f"{prefix}:{text}"


def line_mode(base_url, headers, ssl_ctx, delay, newline):
    print("Line mode — type text and press Enter to send. Ctrl-C to quit.")
    while True:
        try:
            text = input("> ")
        except (KeyboardInterrupt, EOFError):
            print("\nExiting.")
            break

        if not text:
            continue

        cmd = build_type_cmd(text, delay, newline)
        ok, msg = send_command(base_url, headers, cmd, ssl_ctx)
        if not ok:
            print(f"  [error] {msg}", file=sys.stderr)


def keystroke_mode(base_url, headers, ssl_ctx):
    try:
        import tty
        import termios
    except ImportError:
        print("Error: Keystroke mode requires tty/termios (macOS/Linux only).", file=sys.stderr)
        sys.exit(1)

    # Mapping of escape sequences to HID key names
    ESC_MAP = {
        "\x1b[A": "UP",
        "\x1b[B": "DOWN",
        "\x1b[C": "RIGHT",
        "\x1b[D": "LEFT",
        "\x1b[3~": "DELETE",
        "\x1b[H": "HOME",
        "\x1b[F": "END",
        "\x1b[5~": "PAGEUP",
        "\x1b[6~": "PAGEDOWN",
    }

    CHAR_MAP = {
        "\r": "ENTER",
        "\n": "ENTER",
        "\t": "TAB",
        "\x7f": "BACKSPACE",
        "\x1b": "ESC",
    }

    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)

    print("Keystroke mode — keys are sent immediately. Ctrl-C to quit.")
    try:
        tty.setraw(fd)
        while True:
            ch = sys.stdin.read(1)
            if not ch or ch == "\x03":  # Ctrl-C
                break

            # Handle escape sequences (arrow keys, etc.)
            if ch == "\x1b":
                seq = ch
                # Read additional chars with a tiny timeout
                import select
                while True:
                    ready, _, _ = select.select([sys.stdin], [], [], 0.05)
                    if ready:
                        seq += sys.stdin.read(1)
                    else:
                        break

                if seq in ESC_MAP:
                    key = ESC_MAP[seq]
                    send_command(base_url, headers, f"KEY_PRESS:{key}", ssl_ctx)
                    send_command(base_url, headers, f"KEY_RELEASE:{key}", ssl_ctx)
                elif len(seq) == 1:
                    # Bare Escape
                    send_command(base_url, headers, "KEY_PRESS:ESC", ssl_ctx)
                    send_command(base_url, headers, "KEY_RELEASE:ESC", ssl_ctx)
                continue

            # Handle special characters
            if ch in CHAR_MAP:
                key = CHAR_MAP[ch]
                send_command(base_url, headers, f"KEY_PRESS:{key}", ssl_ctx)
                send_command(base_url, headers, f"KEY_RELEASE:{key}", ssl_ctx)
                continue

            # Printable character
            if ch.isprintable():
                send_command(base_url, headers, f"KEY_PRESS:{ch}", ssl_ctx)
                send_command(base_url, headers, f"KEY_RELEASE:{ch}", ssl_ctx)

    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        print("\nExiting.")


def main():
    parser = argparse.ArgumentParser(
        description="Interactive terminal for WiFi USB HID device. "
        "Sends typed text as keyboard input via the REST API.",
        epilog="Examples:\n"
        "  %(prog)s 192.168.4.1\n"
        "  %(prog)s 192.168.1.100 -d 20 --no-newline\n"
        "  %(prog)s 192.168.1.100 -k\n"
        "  %(prog)s --https 192.168.1.100",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("ip", nargs="?", default=None, help="Device IP address (prompted if omitted)")
    parser.add_argument("-u", "--user", default="admin", help="Auth username (default: admin)")
    parser.add_argument("-p", "--password", default="WiFi_HID!826", help="Auth password (default: WiFi_HID!826)")
    parser.add_argument("-d", "--delay", type=int, default=10, help="Delay between keystrokes in ms (default: 10, 0=no delay)")
    parser.add_argument("--no-newline", action="store_true", help="Use TYPE instead of TYPELN (no Enter after each line)")
    parser.add_argument("--https", action="store_true", help="Use HTTPS (self-signed cert, verification disabled)")
    parser.add_argument("-k", "--keystroke", action="store_true", help="Keystroke mode: forward individual key presses in real-time")

    args = parser.parse_args()

    # Prompt for IP if not provided
    ip = args.ip
    if not ip:
        try:
            ip = input("Device IP address: ").strip()
        except (KeyboardInterrupt, EOFError):
            print("\nExiting.")
            sys.exit(0)
        if not ip:
            print("Error: IP address is required.", file=sys.stderr)
            sys.exit(1)

    scheme = "https" if args.https else "http"
    base_url = f"{scheme}://{ip}"
    headers = build_auth_header(args.user, args.password)

    # SSL context for self-signed certs
    ssl_ctx = None
    if args.https:
        ssl_ctx = ssl.create_default_context()
        ssl_ctx.check_hostname = False
        ssl_ctx.verify_mode = ssl.CERT_NONE

    # Verify connectivity with a quick test
    print(f"Connecting to {base_url} ...")
    ok, msg = send_command(base_url, headers, "PING", ssl_ctx)
    if ok:
        print(f"Connected.\n")
    else:
        print(f"Warning: initial ping failed ({msg}). Continuing anyway.\n", file=sys.stderr)

    if args.keystroke:
        keystroke_mode(base_url, headers, ssl_ctx)
    else:
        line_mode(base_url, headers, ssl_ctx, args.delay, not args.no_newline)


if __name__ == "__main__":
    main()
