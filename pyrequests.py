import requests

data = {
    "key1": "value1",
    "key2": "value2"
}
#http://127.0.0.1:8089/index.html?key1=value1&key2=value2

response = requests.post("http://localhost:8089/index.html", data=data)

if response.status_code == 201:
    print("POST request successful!")
    print(response.json())  # Print the JSON response if applicable
else:
    print(f"Request failed with status code: {response.status_code}")

