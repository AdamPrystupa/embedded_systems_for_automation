import requests
URL = 'http://agrosensor.jumpingcrab.com:8051/swa10-task5.php'

try:
    response = requests.get(URL, timeout=10)
    response.raise_for_status()  
    print(f'Statsus odpowiedzi: {response.status_code} ')
    if response.status_code == 200:
        json_data = response.json()
        # print(json_data)
        r = int(json_data['color'][0:2], 16)
        g = int(json_data['color'][2:4], 16)
        b = int(json_data['color'][4:6], 16)
        print(f'R: {r}, G: {g}, B: {b}')
except requests.exceptions.HTTPError as e:
    print(f'HTTPError: {e.response.status_code} - {e.response.reason}')
except requests.exceptions.ConnectionError as e:
    print(f'ConnectionError: {e}')
except requests.exceptions.Timeout as e:
    print(f'Timeout: {e}')
except requests.exceptions.RequestException as e:
    print(f'RequestException: {e}')
except Exception as e:
    print(f'Exception: {e}')