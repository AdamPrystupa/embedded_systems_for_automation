import urequests
from secret import wifi_connect
import json

URL = 'http://10.0.0.198:80/swa10-task5.php?id=275496'

try:
    response = urequests.get(URL, timeout=10)
    print(f'Statsus odpowiedzi: {response.status_code} ')
    if response.status_code == 200:
        json_data = response.json()
        r = int(json_data['color'][0:2], 16)
        g = int(json_data['color'][2:4], 16)
        b = int(json_data['color'][4:6], 16)
        print(f'R: {r}, G: {g}, B: {b}')

        # Zapisz tylko R, G, B do pliku
        rgb_dict = {'R': r, 'G': g, 'B': b}
        try:
            with open('zad16.json', 'w') as f:
                json.dump(rgb_dict, f)
            print("Plik zad16.json zapisany!")
        except Exception as e:
            print(f'Błąd zapisu JSON: {e}')
    response.close()
except Exception as e:
    print(f'Exception: {e}')