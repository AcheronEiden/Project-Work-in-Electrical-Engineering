import csv
import matplotlib.pyplot as plt

#------------------------------------------------------------------------
# DESCRIPTION: Real-time plotting of Voltage 1 and Voltage 2 data 
#   retrieved from a CSV file. The program creates two subplots: 
#   the upper subplot displays the values of Voltage 1 as a function of time, 
#   while the lower subplot shows the values of Voltage 2 as a function of time. 
# RETURNS: Two subplots of Voltage 1 and Voltage 2 data.
# SIDE EFFECTS: Creates two subplots for real-time visualization.
# ASSUMPTIONS: Assumes the CSV file ('logging.csv') contains columns for 'Millis', 
#   'Voltage 1', and 'Voltage 2'. Assumes valid data format and file structure.
# EXAMPLES: None
# RESTRICTIONS: Requires a properly formatted CSV file.
#------------------------------------------------------------------------

# Initialize the lists
time = []
voltage1 = []
voltage2 = []

# Read the file using the csv module
with open('logging.csv', 'r') as csvfile:
    csv_reader = csv.reader(csvfile)
    
    # Iterate through the rows
    for row in csv_reader:
        if row and row[0].startswith('Millis'):
            ms = int(row[0].split(': ')[1])
            v1 = float(row[1].split(': ')[1].replace('V', ''))
            v2 = float(row[2].split(': ')[1].replace('V', ''))
            
            time.append(ms)
            voltage1.append(v1)
            voltage2.append(v2)

# Create two subplots, one above the other
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)

# Set titles for the subplots
ax1.set_title('ECG measurement (lead1) for the patient Alhassan Jawad')
ax2.set_title('ECG measurement (lead2) for the patient Alhassan Jawad')

# Plot the values of voltage 1 as a function of time in the first subplot
ax1.plot(time, voltage1, label="Voltage 1", color='blue')
ax1.set_xlabel('Time (ms)')
ax1.set_ylabel('Voltage (V)')
ax1.legend()

# Plot the values of voltage 2 as a function of time in the second subplot
ax2.plot(time, voltage2, label="Voltage 2", color='red')
ax2.set_xlabel('Time (ms)')
ax2.set_ylabel('Voltage (V)')
ax2.legend()

plt.show()