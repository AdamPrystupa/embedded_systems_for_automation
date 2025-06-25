import urequests
from secret import wifi_connect
import json

URL = 'http://10.0.0.198:80/swa10-task3.php?id=275496'
try:
    response = urequests.get(URL, timeout=10)
    print(f'Statsus odpowiedzi: {response.status_code} ')
    html_content = response.text
    print(html_content[:300])  

    try:
        data = json.loads(html_content)
        with open('/zad14.json', 'w+') as f:
            json.dump(data, f)
    except Exception as e:
        print(f'Błąd zapisu JSON: {e}')

    response.close()
except Exception as e:
    print(f'Exception: {e}')