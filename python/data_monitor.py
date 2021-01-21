import tkinter as tk
import threading
from functools import partial

from ArduinoSerial import *


NUM_COEFFICIENTS = 5
FONT_FAMILY = "Fixedsys"
BACKGROUND_COLOR = "#111111"
GREEN = "#218546"

window = tk.Tk()

# Global widget data
power_button_text = tk.StringVar(value="Power Off")
hold_button_text = tk.StringVar(value="||")
regression_type_button_text = tk.StringVar(value="Use Steinhart-Hart")
coefficient_button_text = tk.StringVar(value="Use New Coefficients")
temperature_entries = []

use_steinhart_hart = False
use_new_coefficients = False

arduino_serial = ArduinoSerial(9600, NUM_COEFFICIENTS)


def str_is_number(string):
	try:
		float(string)
		return True
	except ValueError:
		return False


def reset_variables():
	"""Put all variables back in starting position"""
	global use_steinhart_hart
	global use_new_coefficients

	use_steinhart_hart = False
	regression_type_button_text.set("Use Steinhart-Hart")

	use_new_coefficients = False
	coefficient_button_text.set("Use New Coefficients")

	for text in arduino_serial.calibration_x_text:
		text.set("")
	arduino_serial.coefficient_text.set("")


def toggle_serial():
	"""Open and close the serial port with power button"""
	if arduino_serial.port.is_open:
		power_button_text.set("Power On")
		arduino_serial.should_close = True
		reset_variables()
	else:
		power_button_text.set("Power Off")
		arduino_serial.port.open()


def toggle_hold():
	"""Pause and play data updates from the serial port"""
	if arduino_serial.is_paused:
		hold_button_text.set("||")
	else:
		hold_button_text.set("▶")
	arduino_serial.is_paused = not arduino_serial.is_paused


def submit_temperature(sample_num):
	if not str_is_number(temperature_entries[sample_num].get()):
		return
	command = f"set {sample_num} {temperature_entries[sample_num].get()}"
	arduino_serial.schedule_write(command)


def toggle_regression_type():
	global use_steinhart_hart
	use_steinhart_hart = not use_steinhart_hart
	if use_steinhart_hart:
		regression_type_button_text.set("Use Polynomial")
		arduino_serial.schedule_write("set sh true")
	else:
		regression_type_button_text.set("Use Steinhart-Hart")
		arduino_serial.schedule_write("set sh false")


def toggle_coefficients():
	global use_new_coefficients
	use_new_coefficients = not use_new_coefficients
	if use_new_coefficients:
		coefficient_button_text.set("Use Original Coefficients")
		arduino_serial.schedule_write("set newc true")
	else:
		coefficient_button_text.set("Use New Coefficients")
		arduino_serial.schedule_write("set newc false")


def get_coefficients():
	arduino_serial.schedule_write("get coefficients")


def copy_to_clipboard(event):
	text = event.widget.cget("text")  # Grab label text
	window.clipboard_clear()
	window.clipboard_append(text)


def add_widgets():
	# Data display
	temperature_label = tk.Label(
		window,
		textvariable=arduino_serial.temperature_text,
		font=(FONT_FAMILY, 64, "bold"),
		bg=BACKGROUND_COLOR,
		fg=GREEN,
		pady=68
	)
	temperature_label.pack()

	all_data_label = tk.Label(
		window,
		textvariable=arduino_serial.all_data_text,
		font=(FONT_FAMILY, 17),
		bg=BACKGROUND_COLOR,
		fg=GREEN
	)
	all_data_label.pack()

	# Power and hold buttons
	power_hold_frame = tk.Frame(
		window,
		pady=34,
		bg=BACKGROUND_COLOR
	)
	power_hold_frame.pack()

	power_button = tk.Button(
		power_hold_frame,
		command=toggle_serial,
		textvariable=power_button_text,
		font=(FONT_FAMILY, 16),
		bg="black",
		fg=GREEN,
		activebackground=GREEN,
		activeforeground="black",
		justify=tk.CENTER,
		pady=5,
		padx=5,
		cursor="pirate"
	)
	power_button.pack(side=tk.LEFT)

	hold_button = tk.Button(
		power_hold_frame,
		command=toggle_hold,
		textvariable=hold_button_text,
		font=(FONT_FAMILY, 16, "bold"),
		bg="black",
		fg=GREEN,
		activebackground=GREEN,
		activeforeground="black",
		justify=tk.CENTER,
		pady=5,
		padx=5
	)
	hold_button.pack(side=tk.LEFT)

	# Calibration
	calibration_form_frame = tk.Frame(
		window,
		pady=34,
		bg=BACKGROUND_COLOR
	)
	calibration_form_frame.pack()

	for i in range(NUM_COEFFICIENTS):
		calibration_frame = tk.Frame(
			calibration_form_frame,
			bg=BACKGROUND_COLOR
		)
		calibration_frame.pack()

		temperature_entry = tk.Entry(
			calibration_frame,
			font=(FONT_FAMILY, 17),
			bg="black",
			fg=GREEN,
			width=8,
			justify=tk.CENTER,
			exportselection=0,
			insertbackground=GREEN,
			selectbackground=GREEN,
			selectforeground="black"
		)
		temperature_entry.pack(side=tk.LEFT)
		temperature_entries.append(temperature_entry)

		submit_temperature_button = tk.Button(
			calibration_frame,
			text="Submit",
			command=partial(submit_temperature, i),
			font=(FONT_FAMILY, 16),
			bg="black",
			fg=GREEN,
			activebackground=GREEN,
			activeforeground="black",
			justify=tk.CENTER,
			pady=5,
			padx=5,
		)
		submit_temperature_button.pack(side=tk.LEFT)

		calibration_x_label = tk.Label(
			calibration_frame,
			textvariable=arduino_serial.calibration_x_text[i],
			font=(FONT_FAMILY, 17),
			bg=BACKGROUND_COLOR,
			fg=GREEN
		)
		calibration_x_label.pack(side=tk.LEFT)

	# Calibration setting buttons
	calibration_button_frame = tk.Frame(
		window,
		bg=BACKGROUND_COLOR
	)
	calibration_button_frame.pack()

	regression_type_button = tk.Button(
		calibration_button_frame,
		command=toggle_regression_type,
		textvariable=regression_type_button_text,
		font=(FONT_FAMILY, 16),
		bg="black",
		fg=GREEN,
		activebackground=GREEN,
		activeforeground="black",
		justify=tk.CENTER,
		pady=5,
		padx=5
	)
	regression_type_button.pack(side=tk.LEFT)

	coefficient_button = tk.Button(
		calibration_button_frame,
		command=toggle_coefficients,
		textvariable=coefficient_button_text,
		font=(FONT_FAMILY, 16),
		bg="black",
		fg=GREEN,
		activebackground=GREEN,
		activeforeground="black",
		justify=tk.CENTER,
		pady=5,
		padx=5
	)
	coefficient_button.pack(side=tk.LEFT)

	get_coefficients_button = tk.Button(
		calibration_button_frame,
		command=get_coefficients,
		text="Get Coefficients",
		font=(FONT_FAMILY, 16),
		bg="black",
		fg=GREEN,
		activebackground=GREEN,
		activeforeground="black",
		justify=tk.CENTER,
		pady=5,
		padx=5
	)
	get_coefficients_button.pack(side=tk.LEFT)

	# Coefficients
	coefficient_label = tk.Label(
		window,
		textvariable=arduino_serial.coefficient_text,
		font=(FONT_FAMILY, 17),
		bg=BACKGROUND_COLOR,
		fg=GREEN,
		pady=34
	)
	coefficient_label.pack()
	coefficient_label.bind("<Button-1>", copy_to_clipboard)

	# Change coefficient text on hover
	coefficient_label.bind("<Enter>", lambda event: event.widget.configure(fg="#31a841"))
	coefficient_label.bind("<Leave>", lambda event: event.widget.configure(fg=GREEN))


def main():
	window.winfo_toplevel().title("Detector Serial Monitor")
	window.geometry("1600x800")
	window.configure(bg=BACKGROUND_COLOR)

	add_widgets()

	serial_thread = threading.Thread(target=arduino_serial.run, daemon=True)
	serial_thread.start()

	window.mainloop()


if __name__ == "__main__":
	main()
