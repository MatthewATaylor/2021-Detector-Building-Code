import time
import tkinter as tk

import serial
import serial.tools.list_ports


class ArduinoSerial:
	def __init__(self, baud_rate):
		print("Searching for ports...")
		ports = list(serial.tools.list_ports.comports())
		port_name = ""
		for port in ports:
			print(f"{port.device}: {port.description}")
			if "arduino" in port.description.lower():
				port_name = port.device
		if len(port_name) == 0:
			raise Exception("No Arduino ports found")
		print(f"Selected port: {port_name}\n")
		self.port = serial.Serial(port_name, baud_rate)

		self.should_close = False
		self.is_paused = False

		self.all_data_text = tk.StringVar(value="No data available")
		self.temperature_text = tk.StringVar(value="No data available")
		self.resistance_text = [tk.StringVar() for _ in range(3)]
		self.coefficient_text = [tk.StringVar() for _ in range(3)]

		self.write_queue = []

	def disable_data_text(self):
		self.all_data_text.set("No data available")
		self.temperature_text.set("No data available")

	def run(self):
		while True:
			try:
				if self.should_close:
					self.port.close()
					self.should_close = False
				if self.port.is_open:
					self.read()
					for _ in range(len(self.write_queue)):
						self.port.write(self.write_queue[0])
						del self.write_queue[0]
				else:
					self.disable_data_text()
			except serial.SerialException:
				self.disable_data_text()

	def set_data(self, reading_elements):
		"""Set data label text given list of serial reading_elements"""
		if len(reading_elements) == 1:
			self.all_data_text.set("Loading...")
			self.temperature_text.set("Loading...")
		else:
			all_data_str = ""
			for i, element in enumerate(reading_elements):
				if i == 0:
					continue
				if i == len(reading_elements) - 1:
					# Last element is temperature
					self.temperature_text.set(element)
				else:
					all_data_str += element
					if i != len(reading_elements) - 2:
						all_data_str += "\n"
			self.all_data_text.set(all_data_str)

	def read(self):
		"""Read serial data and update widgets accordingly"""
		reading_elements = str(self.port.readline())[2:-5].split("|")
		reading_elements = [element.strip() for element in reading_elements]
		if reading_elements[0] == "DATA":
			if not self.is_paused:
				self.set_data(reading_elements)
		elif reading_elements[0] == "CALIBRATION" and len(reading_elements) == 3:
			# Example: CALIBRATION | 1 | R = 10000.00
			# reading_elements[1] = sample number
			# reading_elements[2] = thermistor resistance of sample
			if reading_elements[1].isnumeric():
				resistance_str = "  (" + reading_elements[2] + ")"
				self.resistance_text[int(reading_elements[1])].set(resistance_str)
		elif reading_elements[0] == "COEFFICIENTS" and len(reading_elements) == 4:
			# Example: COEFFICIENTS | A = 0.5 | B = 0.5 | C = 0.5
			for i in range(1, 4):
				self.coefficient_text[i - 1].set(reading_elements[i])
		elif reading_elements[0] == "INPUT" and len(reading_elements) == 2:
			# Display Arduino inputs for debugging
			print(f"Arduino received: {reading_elements[1]}")

	def schedule_write(self, value):
		self.write_queue.append(value.encode())
