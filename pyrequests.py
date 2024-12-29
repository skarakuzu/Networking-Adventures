import requests

data = {
    "key1": "value1",
    "mykey2": "thevalue2",
    "otherkey3": "theotherrvalue3",
}
#http://127.0.0.1:8089/index.html?key1=value1&key2=value2

#response = requests.post("http://localhost:8089/index.html", data=data)
response = requests.post("http://localhost:8089/index.html?key4=value4&key5=value5", data=data)

if response.status_code == 200:
    print("POST request successful!")
    print(response.json())  # Print the JSON response if applicable
else:
    print(f"Request failed with status code: {response.status_code}")

