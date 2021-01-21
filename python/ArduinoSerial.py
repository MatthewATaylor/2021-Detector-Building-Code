import time
import tkinter as tk

import serial
import serial.tools.list_ports


class ArduinoSerial:
	def __init__(self, baud_rate, num_coefficients):
		print("Searching for ports...")
		ports = list(serial.tools.list_ports.comports())
		port_name = ""
		for port in ports:
			print(f"{port.device}: {port.description}")
			if "arduino" in port.description.lower():
				port_name = port.device

		self.port = serial.Serial()
		if len(port_name) == 0:
			print("No Arduino ports found")
		else:
			print(f"Selected port: {port_name}\n")
			self.port = serial.Serial(port_name, baud_rate)

		self.num_coefficients = num_coefficients

		self.should_close = False
		self.is_paused = False

		self.all_data_text = tk.StringVar(value="No data available")
		self.temperature_text = tk.StringVar(value="No data available")
		self.calibration_x_text = [tk.StringVar() for _ in range(num_coefficients)]
		self.coefficient_text = tk.StringVar()

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
			time.sleep(0.1)

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
		elif reading_elements[0] == "CALIBRATION" and len(reading_elements) == 4:
			# Example: CALIBRATION | 1 | V = 1.5 | R = 10000
			# reading_elements[1] = sample number
			# reading_elements[2] = thermistor voltage of sample
			# reading_elements[3] = thermistor resistance of sample
			if reading_elements[1].isnumeric():
				calibration_x_str = \
					"  (" + reading_elements[2] + ", " + reading_elements[3] + ")"
				self.calibration_x_text[int(reading_elements[1])].set(calibration_x_str)
		elif reading_elements[0] == "COEFFICIENTS" and \
				len(reading_elements) == self.num_coefficients + 1:
			# Example: COEFFICIENTS | 0.5 | 0.5 | 0.5 | 0.0 | 0.0
			coefficient_str = ""
			for i in range(1, self.num_coefficients + 1):
				# Format coefficients for easy pasting into Arduino code
				coefficient_str += reading_elements[i]
				if i != self.num_coefficients:
					coefficient_str += ",\n"
			self.coefficient_text.set(coefficient_str)
		elif reading_elements[0] == "INPUT" and len(reading_elements) == 2:
			# Display Arduino inputs for debugging
			print(f"Arduino received: {reading_elements[1]}")

	def schedule_write(self, value):
		self.write_queue.append(value.encode())
