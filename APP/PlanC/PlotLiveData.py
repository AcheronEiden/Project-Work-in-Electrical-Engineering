import serial
import csv
import matplotlib.pyplot as plt
from collections import deque

#------------------------------------------------------------------------
# DESCRIPTION: Real-time plotting and logging of Voltage 1 and Voltage 2 data
#   received from a serial port. The data is visualized in two subplots, with
#   Voltage 1 plotted against time in the upper subplot, and Voltage 2 in the 
#   lower subplot. The program also saves the data to a CSV file named 'logging.csv'
#   when interrupted by a KeyboardInterrupt.
# RETURNS: Real-time interactive plot and logging of Voltage 1 and Voltage 2 data.
# SIDE EFFECTS: Creates two subplots for real-time visualization and saves data to 'logging.csv'.
# ASSUMPTIONS: Assumes the incoming data format is a string containing time, Voltage 1,
#   and Voltage 2 values separated by ', '. Assumes the serial port is connected to
#   a device providing these values.
# EXAMPLES: None
# RESTRICTIONS: Requires a valid serial port connection and proper data format.
#------------------------------------------------------------------------

# Initialize serial communication
ser = serial.Serial('COM6', 115200)

# Set the duration to keep in memory (in milliseconds)
memory_duration = 100

# Initialize lists with deque to efficiently manage the data
time_data = deque(maxlen=memory_duration)
voltage1_data = deque(maxlen=memory_duration)
voltage2_data = deque(maxlen=memory_duration)

# Enable interactive mode for real-time plotting
plt.ion()

# Create two subplots, one above the other
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)

try:
    while True:
        # Read data from the serial port
        data = ser.readline().decode('utf-8').strip()

        # Split the data into time, voltage1, and voltage2
        millis, voltage1, voltage2 = map(float, data.split(', '))

        # Append data to lists
        time_data.append(millis)
        voltage1_data.append(voltage1)
        voltage2_data.append(voltage2)

        # Plot the data in the first subplot (Voltage 1 vs Time)
        ax1.clear()
        ax1.plot(time_data, voltage1_data, label='Voltage 1')

        ax1.legend()
        ax1.set_title('ECG measurement (lead1) for the patient Alhassan Jawad')
        #ax1.set_xlabel('Time (ms)')
        ax1.set_ylabel('Voltage (V)')
        ax1.set_ylim(1, 5)

        # Plot the data in the second subplot (Voltage 2 vs Time)
        ax2.clear()
        ax2.plot(time_data, voltage2_data, label='Voltage 2')
        ax2.legend()
        ax2.set_title('ECG measurement (lead2) for the patient Alhassan Jawad')
        ax2.set_xlabel('Time (ms)')
        ax2.set_ylabel('Voltage (V)')
        ax2.set_ylim(0, 5)

        # Pause to allow for real-time plotting
        plt.pause(0.001)

except plt.GettingClobbered:
    # Close the serial port and plot
    ser.close()
    print("Serial port closed.")
    plt.close()
    print("Plot closed.")

    # Save data to CSV file
    with open('logging.csv', 'w', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        csv_writer.writerow(['Millis', 'Voltage 1', 'Voltage 2'])
        for millis, v1, v2 in zip(time_data, voltage1_data, voltage2_data):
            csv_writer.writerow([millis, f"Voltage 1: {v1:.2f}V", f"Voltage 2: {v2:.2f}V"])

    print("Data saved to logging.csv.")