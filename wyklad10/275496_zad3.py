import urllib.request
import urllib.error

URL = 'http://agrosensor.jumpingcrab.com:8051/swa10-task3.php'

try:
    with urllib.request.urlopen(URL, timeout=10) as response:
        print(f'Statsus odpowiedzi: {response.status} {response.reason}')
        if response.status == 200:
            content = response.read()
            charset = response.headers.get_content_charset()
            if charset:
                content_string = content.decode(charset)
            else:
                content_string = content.decode('utf-8')
            print(content_string[:300])
        else:
            print(f'Błąd pobierania danych. Kod błędu: {response.status} ')
except urllib.error.HTTPError as e:
    print(f'HTTPError: {e.code} - {e.reason}')
except urllib.error.URLError as e:
    print(f'URLError: {e.reason}')
except TimeoutError:
    print('TimeoutError')
except Exception as e:
    print(f'Inny błąd: {e}')