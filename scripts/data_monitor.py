import tkinter as tk
import threading

import serial

window = tk.Tk()
window.winfo_toplevel().title("Detector Serial Monitor")
window.geometry("1600x800")
window.configure(bg="black")

serial_reading = tk.StringVar()
serial_port = serial.Serial("COM3", 9600)

def read_serial():
	while True:
		reading = str(serial_port.readline())
		reading = reading[2:-5]
		serial_reading.set(reading)

serial_thread = threading.Thread(target=read_serial, daemon=True)
serial_thread.start()

debug_label = tk.Label(
	window,
	textvariable=serial_reading,
	font=("Fixedsys", 18),
	bg="black",
	fg="#258a40"
)
debug_label.pack()

temperature_label = tk.Label(
	window,
	textvariable=serial_reading,
	font=("Fixedsys", 18),
	bg="black",
	fg="#258a40"
)
temperature_label.pack()

window.mainloop()
