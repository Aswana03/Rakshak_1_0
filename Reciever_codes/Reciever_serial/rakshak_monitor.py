import serial
import csv
import webbrowser
import requests
from plyer import notification

ser = serial.Serial('COM5', 115200)   # CHANGE COM PORT
last_message = ""

def get_owner(device_id):
    with open('users.csv') as file:
        for row in csv.reader(file):
            if row[0] == device_id:
                return row[1]
    return "Unknown User"

print("RAKSHAK 1.0 Monitoring System Running...")

while True:
    line = ser.readline().decode(errors='ignore').strip()

    if "id:" in line and line != last_message:
        last_message = line

        print("\nReceived:", line)

        parts = line.split(",")

        device_id = parts[0].replace("id:", "")
        event = parts[1]

        DEFAULT_LAT = "10.8238"
        DEFAULT_LON = "76.6426"

        if parts[2] == "NO_GPS":
            print("⚠ GPS not available — using default location")
            lat = DEFAULT_LAT
            lon = DEFAULT_LON
        else:
            lat = parts[2]
            lon = parts[3]

        owner = get_owner(device_id)

        print("Owner:", owner)
        print("Latitude:", lat)
        print("Longitude:", lon)

        map_link = f"https://www.openstreetmap.org/?mlat={lat}&mlon={lon}"
        webbrowser.open(map_link)

        try:
            geo = requests.get(
                f"https://nominatim.openstreetmap.org/reverse?format=json&lat={lat}&lon={lon}",
                headers={"User-Agent": "RakshakSystem"}
            ).json()

            address = geo.get("display_name", "Location not found")

        except:
            address = "Location lookup failed (network busy)"

        print("Location:", address)

        if event == "P01":
            msg = f"Panic detected from {owner}"
        else:
            msg = f"Emergency from {owner}"

        notification.notify(
            title="RAKSHAK ALERT",
            message=msg,
            timeout=10
        )