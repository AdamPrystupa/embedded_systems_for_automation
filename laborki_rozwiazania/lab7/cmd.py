import sys
from ws2812b import ws_fill, ws_show

def hex_to_rgb(hexstr):
    """Convert 6-char hex string to (R, G, B) tuple."""
    hexstr = hexstr.lstrip('#')
    if len(hexstr) != 6:
        raise ValueError("Color must be 6 hex digits")
    r = int(hexstr[0:2], 16)
    g = int(hexstr[2:4], 16)
    b = int(hexstr[4:6], 16)
    return (r, g, b)

def main():
    print("Simple Terminal. Type 'help' for commands. Type 'exit' to quit.")
    while True:
        try:
            command = input("> ").strip()
            if not command:
                continue

            parts = command.split()
            cmd = parts[0]
            args = parts[1:]

            if cmd == "exit":
                print("Exiting terminal.")
                break

            elif cmd == "df":
                if len(args) != 1:
                    print("Usage: df XXXXXX (hex color)")
                else:
                    try:
                        color = hex_to_rgb(args[0])
                        ws_fill(color)
                        ws_show()
                        print(f"Set df color to: {args[0].upper()} (RGB: {color})")
                    except Exception as e:
                        print(f"Invalid color: {e}")

            elif cmd == "connect":
                # TODO: Implement connect
                print("[TODO] Connecting to server...")

            elif cmd == "disconnect":
                # TODO: Implement disconnect
                print("[TODO] Disconnecting from server...")

            elif cmd == "weather":
                # TODO: Implement weather
                print("[TODO] Fetching weather data...")

            elif cmd == "oled":
                # TODO: Implement oled wiersz tekst
                print(f"[TODO] Display on OLED row {args[0]}: {' '.join(args[1:])}")

            elif cmd == "color":
                # TODO: Implement color
                print("[TODO] Fetching and updating color...")

            elif cmd == "tcp":
                # TODO: Implement tcp komenda argumenty
                print(f"[TODO] Sending TCP command: {' '.join(args)}")

            elif cmd == "help":
                print("Available commands:")
                print("  df XXXXXX         - Set df to color XXXXXX")
                print("  connect           - Connect to server")
                print("  disconnect        - Disconnect from server")
                print("  weather           - Get weather info")
                print("  oled wiersz tekst - Display text on OLED row")
                print("  color             - Get and update color")
                print("  tcp ...           - Send TCP command")
                print("  exit              - Exit terminal")

            else:
                print(f"Unknown command: {cmd}")

        except (KeyboardInterrupt, EOFError):
            print("\nExiting terminal.")
            break

if __name__ == "__main__":
    main()
