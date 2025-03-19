# Fixed values
fixed_values = {
    "SkipFcntChk": "True",          # Example fixed value for SkipFcntChk
    "IsDisable": "False",           # Example fixed value for IsDisable
    "Protocol": "ABP104",          # Example fixed value for Protocol
    "AppID": "c3c81a7d-6452-4c49-8061-91a56477751d",               # Example fixed value for AppID
    "DevProID": "3a9f7caa-fa6f-446c-9454-0e973f02ff0f",            # Example fixed value for DevProID
    "JoinEUI": "0011223344556677"   # Example fixed value for JoinEUI
}
# Starting number for sequential Name
start_number = 121  # Set your desired start number here

import csv
from datetime import datetime

# File paths
input_filename = "/Users/living/Documents/Arduino/sq_xda003b_v0/src.txt"
output_filename = "/Users/living/Documents/Arduino/sq_xda003b_v0/device_list.csv"

# Define the extended columns
columns = [
    "Name", "Desc", "Protocol", "AppID", "DevProID", "DevEUI", "JoinEUI",
    "DevAddr", "NwkSKey", "AppSKey", "SNwkSKey", "FNwkSKey", "AppKey",
    "NwkKey", "SkipFcntChk", "IsDisable"
]

# Get today's date in YYYYMMDD format
current_date = datetime.now().strftime("%Y%m%d")

# Counter for sequential Name generation
name_counter = start_number

# Initialize a list to hold the parsed data
data_list = []

# Read and process the input file
try:
    with open(input_filename, mode='r') as file:
        lines = file.readlines()
except FileNotFoundError:
    print(f"Error: File '{input_filename}' not found.")
    exit(1)

# Parse lines to extract relevant data
current_data = {}
for line in lines:
    line = line.strip()
    if "---- Opened the serial port" in line:
        # Process the previous block if it exists
        if current_data:
            # Generate sequential Name with date
            current_data["Name"] = f"{current_date}{name_counter:03}"  # Format: YYYYMMDD###
            name_counter += 1  # Increment the counter

            # Add fixed values and missing fields with empty values
            for fixed_key, fixed_value in fixed_values.items():
                current_data[fixed_key] = fixed_value
            data_list.append({col: current_data.get(col, "") for col in columns})

            # Reset current_data for the next block
            current_data = {}
    
    if "=" in line or ":" in line:
        if "=" in line:
            key, value = line.split('=', 1)
        elif ":" in line:
            key, value = line.split(':', 1)

        # Map specific keys to corresponding columns
        if key == "APPxKEY":
            current_data["AppSKey"] = value
            current_data["AppKey"] = value
        elif key == "NWKxKEY":
            current_data["NwkSKey"] = value
            current_data["SNwkSKey"] = value
            current_data["FNwkSKey"] = value
            current_data["NwkKey"] = value
        elif key == "DEVADDR":
            current_data["DevAddr"] = value
        elif key == "DEVEUI":
            current_data["DevEUI"] = value
        elif key == "ESP32ChipID":
            # Generate Desc based on the first 6 characters of ESP32ChipID
            current_data["Desc"] = f"SQ-{value[:6]}"

# Process the last block if it exists
if current_data:
    current_data["Name"] = f"{current_date}{name_counter:03}"
    name_counter += 1
    for fixed_key, fixed_value in fixed_values.items():
        current_data[fixed_key] = fixed_value
    data_list.append({col: current_data.get(col, "") for col in columns})

# Write the parsed data to a CSV file
with open(output_filename, mode='w', newline='') as csvfile:
    csv_writer = csv.DictWriter(csvfile, fieldnames=columns)
    csv_writer.writeheader()
    csv_writer.writerows(data_list)

print(f"Data successfully written to {output_filename}.")
