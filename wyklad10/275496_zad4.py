import requests
URL = 'http://agrosensor.jumpingcrab.com:8051/swa10-task3.php'

try:
    response = requests.get(URL, timeout=10)
    response.raise_for_status()  
    print(f'Statsus odpowiedzi: {response.status_code} ')
    html_content = response.text
    print(html_content[:300])  
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