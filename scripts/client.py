import random
import requests
import time

server_address = 'http://localhost:12345'

keys = ['key1', 'key2', 'key3', 'key4', 'key5', 'key6', 'key7', 'key8', 'key9']


def send_get_request(key):
    url = f"{server_address}/get_key?key={key}"
    try:
        response = requests.get(url)
        print(f"GET request for key '{key}': {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"Error sending GET request for key '{key}': {e}")

def send_set_request(key, value):
    url = f"{server_address}/set_key"
    data = '{' + f'"{key}":"{value}"' + '}'
    try:
        response = requests.post(url, data=data)
        print('wtf', response.request.url, response.request.body)
        print(f"POST request to set key '{key}' to value '{value}': {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"Error sending POST request to set key '{key}' to value '{value}': {e}")

def main():
    max_reconnects = 10
    current_reconnect = 0
    total_requests = 10000
    while total_requests > 0:
        try:
            key = random.choice(keys)
            if random.random() < 0.99:
                send_get_request(key)
            else:
                value = random.randint(1, 1000)
                send_set_request(key, str(value))

            total_requests -= 1
            current_reconnect = 0

        except KeyboardInterrupt:
            print("Client stopped.")
            break
        except Exception as e:
            print(f"Unexpected error occurred: {e}")
            print("Reconnecting...")
            current_reconnect += 1
            if current_reconnect > max_reconnects:
                break

            time.sleep(10)  # Wait for 1 second before reconnecting

if __name__ == "__main__":
    main()