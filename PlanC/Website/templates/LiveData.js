// Function to update the live plot
function updatePlot(data) {
    // Extract the time, voltage1, and voltage2 from the data string
    const [millis, voltage1, voltage2] = data.split(', ').map(parseFloat);

    // Update the plot using Plotly
    Plotly.extendTraces('live-plot', { x: [[millis]], y: [[voltage1]] }, [0]);
    Plotly.extendTraces('live-plot', { x: [[millis]], y: [[voltage2]] }, [1]);
}

// Set up the initial plot layout
const layout = {
    title: 'Live ECG Plot',
    xaxis: { title: 'Time (ms)' },
    yaxis: { title: 'Voltage (V)', range: [0, 5] }
};

// Set up the initial plot data
const initialData = [
    { x: [], y: [], mode: 'lines+markers', name: 'Voltage 1', line: { color: 'blue' } },
    { x: [], y: [], mode: 'lines+markers', name: 'Voltage 2', line: { color: 'red' } }
];

// Create the live plot using Plotly
Plotly.newPlot('live-plot', initialData, layout);

// Connect to the serial port using WebSocket (replace 'ws://localhost:8000' with your server)
const ws = new WebSocket('ws://localhost:8000');

// Handle messages received from the server
ws.onmessage = function (event) {
    // Update the plot with the new data
    updatePlot(event.data);
};