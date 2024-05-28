from tkinter import *
import serial
import serial.tools.list_ports
import time
import threading
from tkinter import messagebox

# Main loop
# Creates the GUI Interface
root = Tk()
print("Initialising GUI")
# Create serial port
ser = serial.Serial()

# Fixed dimensions for the GUI
root.geometry("900x200")  # Updated dimension
root.resizable(0, 0)  # Fixed dimension (Not resizable)

# Creating a Canvas and placing it with .grid()
canvasMain = Canvas(root, height=600, width=800)
canvasMain.grid()

# Header Texts
waveTypeLB = Label(canvasMain, text="Wave Type")
waveTypeLB.grid(row=1, column=1)

waveLB = Label(canvasMain, text="Wave Parameters")
waveLB.grid(row=1, column=2, columnspan=2)

valuesLB = Label(canvasMain, text="Values")
valuesLB.grid(row=1, column=4)

WavedataLB = Label(canvasMain, text="Current Wavedata")
WavedataLB.grid(row=1, column=6)

# List box for the wave types
lb = Listbox(canvasMain, width=25, height=8)
lb.insert(1, "DC (Default)")
lb.insert(2, "Sine")
lb.insert(3, "Square")
lb.insert(4, "Triangle")
lb.insert(5, "Sawtooth/Ramp")
lb.insert(6, "Reverse Sawtooth/Ramp")
# Place the listbox into the grid
lb.grid(row=2, column=1, rowspan=8)

# Frequency / Period GUI
# Create variables for the radio button
fpVar = IntVar()  # fp -> frequency/period
fpVar.set(1)  # Default Setting is Frequency

# Create a radio button for frequency and period with the default being frequency
freqRB = Radiobutton(canvasMain, text="Frequency", variable=fpVar, value=1)
freqRB.grid(row=2, column=2, sticky=W)
perRB = Radiobutton(canvasMain, text="Period", variable=fpVar, value=2)
perRB.grid(row=2, column=3, sticky=W)
# Entry which correlates to frequency/period
entryFP = Entry(canvasMain)
entryFP.grid(row=2, column=4)

# Amplitude / Peak-to-Peak GUI
# Create variables for the radio button
apVar = IntVar()  # ap -> amplitude/peak-to-peak
apVar.set(1)  # Default Setting is Amplitude

# Creates a radio button for amplitude and peak-to-peak with the default being amplitude


# Wave Offset GUI
# Create a label for the wave offset
waveOSLB = Label(canvasMain, text="Wave Offset")
waveOSLB.grid(row=4, column=3)
# Create a entry for the wave offset

# Wave Function State GUI
# Create a label for the function state
waveState = Label(canvasMain, text="Function Generator State")
waveState.grid(row=6, column=2, columnspan=2)
# Create a variable for the wave state
waveStateVar = IntVar()
waveStateVar.set(1)  # Default Setting is ON
# Create radio buttons for the wave state
waveOnRB = Radiobutton(canvasMain, text="On", variable=waveStateVar, value=1)
waveOnRB.grid(row=6, column=4, sticky=W)
waveOffRB = Radiobutton(canvasMain, text="Off", variable=waveStateVar, value=2)
waveOffRB.grid(row=6, column=5, sticky=W)


def create_amplitude_options():
    # Creating amplitude options from 0V to 5V with 50mV steps
    return ["{:.2f} ".format(x * 0.05) for x in range(101)]


def create_peak_to_peak_options():
    # Creating peak-to-peak options from -5V to 5V with 100mV steps
    return ["{:.2f} ".format(x * 0.1 - 5) for x in range(101)]


def create_offset_options():
    # Creating offset options from -5V to 5V with 100mV steps
    return ["{:.2f} ".format(x * 0.1 - 5) for x in range(101)]


amplitudeVar = StringVar()
peakToPeakVar = StringVar()
offsetVar = StringVar()

ampMenu = OptionMenu(canvasMain, amplitudeVar, *create_amplitude_options())
ptpMenu = OptionMenu(canvasMain, peakToPeakVar, *create_peak_to_peak_options())
offsetMenu = OptionMenu(canvasMain, offsetVar, *create_offset_options())
offsetMenu.grid(row=4, column=4)

ampMenu.grid(row=3, column=4)
ptpMenu.grid_remove()  # Hiding the peak-to-peak menu at initialisation

# Creating radio buttons to control the display
ampRB = Radiobutton(canvasMain, text="Amplitude", variable=apVar, value=1)
ampRB.grid(row=3, column=2, sticky=W)
ptpRB = Radiobutton(canvasMain, text="Peak-to-Peak", variable=apVar, value=2)
ptpRB.grid(row=3, column=3, sticky=W)


# Updated display drop-down menu
def update_menu_display():
    if apVar.get() == 1:  # Amplitude is selected
        ampMenu.grid()
        ptpMenu.grid_remove()
    else:  # Peak to Peak is selected
        ampMenu.grid_remove()
        ptpMenu.grid()


# This section finds the active COMS connected to the device
active_coms = []
chosen_com = StringVar()

voltage_label = Label(canvasMain, text="Voltage: 0 V")
voltage_label.grid(row=2, column=6)
peak_to_peak_label = Label(canvasMain, text="Peak to Peak: 0 V")
peak_to_peak_label.grid(row=2, column=7)
frequency_label = Label(canvasMain, text="Frequency: 0 Hz")
frequency_label.grid(row=3, column=6)
period_label = Label(canvasMain, text="Period: 0 us")
period_label.grid(row=3, column=7)
waveform_label = Label(canvasMain, text="Waveform: None")
waveform_label.grid(row=4, column=6)
offset_label = Label(canvasMain, text="Offset: 0 V")
offset_label.grid(row=5, column=6)


def get_ports():
    count = 0
    active_coms.clear()  # Clear array
    for x in serial.tools.list_ports.comports():  # Import active COMS into the array
        port = x.device
        active_coms.append(port)
        count += 1
    # If there were no active COMS found, variable is set to None.
    if (active_coms == []):
        chosen_com.set("None")


# When GUI.py first initialises, run get_ports() once to get any initial active ports
get_ports()
# If none were detected, default the variable to "None" else, choose the first one found.
if (active_coms == []):
    chosen_com.set("None")
else:
    chosen_com.set(active_coms[0])
# Creates a option menu for the active COMS where you can select them

com_menu = OptionMenu(canvasMain, chosen_com, *active_coms, "None")
com_menu.grid(row=12, column=4)


def toggle_serial_connection():
    if ser.is_open:
        try:
            ser.close()
            ser.port = chosen_com.get()
            ser.baudrate = 19200
            ser.timeout = 2
            ser.open()
            print("port has been connected")
            output = "c"
            ser.write(output.encode('utf-8'))
        except Exception as e:
            print(f" error: {e}")
    else:
        try:
            ser.port = chosen_com.get()
            ser.baudrate = 19200
            ser.timeout = 2
            ser.open()
            output = "c"
            ser.write(output.encode('utf-8'))
            print("port has been connected")
        except Exception as e:
            print(f"unable to open: {e}")


connect_button = Button(canvasMain, text="connect", command=toggle_serial_connection)
connect_button.grid(row=12, column=5)


def refresh_ports():
    # Get the currently available COM ports
    active_coms.clear()  # Clear the old port list
    for port_info in serial.tools.list_ports.comports():
        active_coms.append(port_info.device)
    # Update options in drop-down menus
    com_menu['menu'].delete(0, 'end')  # Clear the existing options in the drop-down menu
    for port in active_coms:
        com_menu['menu'].add_command(label=port, command=lambda value=port: chosen_com.set(value))

    com_menu['menu'].add_command(label="None", command=lambda: chosen_com.set("None"))
    # If no port is detected, "None" is displayed by default.
    if not active_coms:
        chosen_com.set("None")
    else:
        chosen_com.set(active_coms[0])


# Add a refresh button, located next to the drop-down menu
refresh_button = Button(canvasMain, text="Refresh", command=refresh_ports)
refresh_button.grid(row=12, column=2)


# Synchronise Button Function
# Synchronise() gets all the data from the GUI and places it all into one string at the end
def synchronise():
    # Get the wave type
    if lb.get(ANCHOR) == "DC (Default)":
        waveType = '1'
    elif lb.get(ANCHOR) == "Sine":
        waveType = '2'
    elif lb.get(ANCHOR) == "Square":
        waveType = '3'
    elif lb.get(ANCHOR) == "Triangle":
        waveType = '4'
    elif lb.get(ANCHOR) == "Sawtooth/Ramp":
        waveType = '5'
    elif lb.get(ANCHOR) == "Reverse Sawtooth/Ramp":
        waveType = '6'
    else:
        print("Error: Invalid WaveType")

    # Get the Amplitude/Peak-to-Peak selection
    ap = apVar.get()
    # Get the entry for the Amplitude/Peak-to-Peak data
    if ap == 1:  # Amplitude selected
        eAP = amplitudeVar.get()
    else:  # Peak-to-Peak selected
        eAP = peakToPeakVar.get()
    # print("0->None, 1-> AMP, 2->Peak: " + str(ap) + "\nEntry: " + eAP) # For debugging purposes

    # Set amplitude default value to '_', if it comes up in the final string,
    # it means there was no entry for the amplitude/peak-to-peak value
    amplitude = '????'
    try:
        eAP = float(eAP)  # Change string to float value
        # Convert the data to amplitude (consistent formatting)
        if ap == 2:  # Peak-to-Peak was selected, convert to amplitude formatting
            if eAP < 0:
                # Change negative to positive value
                eAP *= -1
            # Rounds the number to be in intervals of 0.1
            amplitude = round(float(eAP), 1)
        else:
            # Rounds the number to be intervals of 0.05
            amplitude = float(eAP)
            amplitude *= 2
            amplitude = round(amplitude, 1)
            amplitude /= 2

        # Restricting upper and lower bounds for amplitude
        if (amplitude > 5):
            amplitude = 5  # Cannot go higher than 5
        elif (amplitude < 0):
            amplitude = 0  # Cannot go lower than 0
        amplitude = format(amplitude, '.2f')
    except ValueError:  # if no numeric value in the eAP entry
        print("Invalid Amplitude/Peak-to-Peak Value")

    # Get the frequency/period selection
    fp = fpVar.get()
    eFP = entryFP.get()  # Retrieve data from the entry
    # print("0-> None, 1-> Freq, 2-> Period: " + str(fp) + "\nEntry: " + eFP) # Used for debugging purposes
    frequency = '?????'  # Default value
    try:
        eFP = float(eFP)
        # Convert the data to amplitude (consistent formatting)
        if (fp == 2):  # Period was selected, so convert to frequency
            frequency = int(1 / eFP)
        else:
            frequency = int(eFP)

        # Defining upper and lower bound limits
        if (frequency > 20000):
            frequency = 20000  # Cannot go higher than 20,000 Hz
        elif (frequency < 1):
            frequency = 1  # Cannot go lower than 1 Hz

        frequency = ("%05d" % frequency)

    except ValueError:  # if no numeric value in the eAP entry
        print("Invalid Frequency/Period Parameter")

    # Getting the wave offset data
    eOS = offsetVar.get()
    offset = '????'

    try:
        # Rounding offset in intervals of 0.1
        offset = round(float(eOS), 1)
        sign = '+'
        if (offset < 0):
            sign = ''

        # Restrict the upper and lower bounds
        if (offset > 5):
            offset = 5  # Cannot go higher than 5
        elif (offset < -5):
            offset = -5  # Cannot go lower than -5
        print("offset: " + str(offset))
        offset = sign + format(offset, '.1f')
    except ValueError:
        # Invalid data from the entry
        print("Invalid wave offset value")

    # Combine to get the final output string
    output_str = "T=" + waveType + "=F=" + frequency + "=A=" \
                 + amplitude + "=O=" + offset + "=S=" \
                 + str(waveStateVar.get()) + '\n'
    print(output_str)

    # Set the serial port parameters
    if ser.port != chosen_com.get():
        ser.close()
        ser.port = chosen_com.get()
        ser.baudrate = 19200
    try:
        ser.open()
    except serial.SerialException:
        print("fail")
    # Send out final string with UART
    if ser.is_open:
        ser.write(output_str.encode('utf-8'))
        print(output_str)
        print("successful")
    else:
        print("No COMS are open")
    # Refresh the ports in the DropBox
    # get_ports()


# Sync Button in the GUI
syncButton = Button(canvasMain, text="Synchronise", command=synchronise)
syncButton.grid(row=12, column=3)

# Frequency/Period Unit Labeling
freqUnitLB = Label(canvasMain, text="Hz")
freqUnitLB.grid(row=2, column=5)
perUnitLB = Label(canvasMain, text="s")
perUnitLB.grid(row=2, column=5)

# Amplitude/peak-to-peak unit labeling
ampUnitLB = Label(canvasMain, text="V")
ampUnitLB.grid(row=3, column=5)
ptpUnitLB = Label(canvasMain, text="Vpp")
ptpUnitLB.grid(row=3, column=5)


def update_fp_unit():
    if fpVar.get() == 1:  # If the frequency is selected, remove cycle unit
        freqUnitLB.grid(row=2, column=5)
        perUnitLB.grid_remove()
    else:  # If the cycle is selected ,remove frequency unit
        freqUnitLB.grid_remove()
        perUnitLB.grid(row=2, column=5)


def update_ap_unit():
    if apVar.get() == 1:  # If amplitude is selected, remove Peak to Peak unit
        ampUnitLB.grid(row=3, column=5)
        ptpUnitLB.grid_remove()
        ptpMenu.grid_remove()
        ampMenu.grid(row=3, column=4)
    else:  # If Peak to Peak is selected, remove amplitude unit
        ampUnitLB.grid_remove()
        ptpUnitLB.grid(row=3, column=5)
        ampMenu.grid_remove()
        ptpMenu.grid(row=3, column=4)


freqRB = Radiobutton(canvasMain, text="Frequency", variable=fpVar, value=1, command=update_fp_unit)
freqRB.grid(row=2, column=2, sticky=W)
perRB = Radiobutton(canvasMain, text="Period", variable=fpVar, value=2, command=update_fp_unit)
perRB.grid(row=2, column=3, sticky=W)

# Create a radio button for amplitude and peak-to-peak values, and bind a function to update the unit labels
ampRB = Radiobutton(canvasMain, text="Amplitude", variable=apVar, value=1, command=update_ap_unit)
ampRB.grid(row=3, column=2, sticky=W)
ptpRB = Radiobutton(canvasMain, text="Peak-to-Peak", variable=apVar, value=2, command=update_ap_unit)
ptpRB.grid(row=3, column=3, sticky=W)

# Adding offset unit labels
waveOSUnitLB = Label(canvasMain, text="V")
waveOSUnitLB.grid(row=4, column=5)


def uart_init(port):
    # Close the serial port if already open
    if ser.is_open:
        ser.close()
    port = chosen_com.get()
    # Set the serial port parameters
    ser.baudrate = 19200
    ser.port = port
    ser.timeout = 1  # overtime of write
    ser.write_timeout = 1  # set overtime write
    # If the port was set to None, do nothing and return.
    if (port == "None"):
        print("Nothing is connected")
        return

    try:
        ser.open()
    except serial.SerialException:
        # If you cannot open the port, print this.
        print("No SeeeduinoXiao detected!")


connection_lost_shown = False


def read_from_port(ser):
    global connection_lost_shown
    while True:
        try:
            # check whether the port is open
            if ser.is_open:
                line = ser.readline()
                if line:
                    line = line.decode('utf-8', errors='ignore').strip()
                    if line == "C":
                        print("connecting")
                    else:
                        parse_and_update_display(line)
                    print("Received:", line)
                    connection_lost_shown = False
            else:
                # attempt to reopen port
                print("Attempting to reconnect...")
                time.sleep(1)
                ser.port = chosen_com.get() if chosen_com.get() != "None" else None
                if ser.port:
                    ser.baudrate = 19200
                    ser.timeout = 0.5
                    ser.open()
                connection_lost_shown = False
        except serial.SerialException as e:
            # deal with port error
            print("Serial Exception:", e)
            if not connection_lost_shown:
                messagebox.showwarning("Warning", "Connection Lost")
                connection_lost_shown = True
            ser.close()
            time.sleep(0.5)
        except Exception as e:
            # deal other errors
            print("An error occurred:", e)
            break
        # delay 0.1s
        time.sleep(0.1)


def parse_and_update_display(data):
    # Processing data
    parts = data.split('=')
    if len(parts) < 8:  # Data checking
        print("Incomplete data received:", data)
        return

    # Maps the number of a waveform type to a specific name
    wave_type_map = {'1': 'DC', '2': 'Sine', '3': 'Square', '4': 'Triangle', '5': 'Sawtooth', '6': 'Reverse Sawtooth'}
    wave_type = wave_type_map.get(parts[1], 'Unknown')
    frequency = float(parts[3])  # frequency
    amplitude = float(parts[5])  # amplitude
    offset = parts[7]  # offset
    peak_to_peak = 2 * amplitude
    period = 1 / frequency * 1000000

    offset = offset.split('\n')[0]

    update_received_data_display(amplitude, frequency, wave_type, offset, peak_to_peak, period)
    # Updating the GUI display in the main thread
    root.after(0, update_received_data_display, amplitude, frequency, wave_type, offset, peak_to_peak, period)


def update_received_data_display(voltage, frequency, waveform, offset, peak_to_peak, period):
    global voltage_label, frequency_label, waveform_label, offset_label, peak_to_peak_label, period_label
    voltage_label.config(text=f"Voltage: {voltage} V")
    peak_to_peak_label.config(text=f"Peak to Peak: {peak_to_peak} V")
    frequency_label.config(text=f"Frequency: {frequency} Hz")
    period_label.config(text=f"Period: {period:.1f} us")
    waveform_label.config(text=f"Waveform: {waveform}")
    offset_label.config(text=f"Offset: {offset} V")


def send_periodic_data():
    if ser.is_open:
        try:
            ser.write("c".encode('utf-8'))  # send string "c"
            print("Data 'c' sent to the serial port.")
        except serial.SerialException as e:
            print(f"Error sending data: {e}")
            ser.close()

    # next send
    root.after(1000, send_periodic_data)


def auto_connect():
    # Get all currently available serial ports
    available_ports = [comport.device for comport in serial.tools.list_ports.comports()]
    # If there is no open serial connection
    if not ser.is_open:
        # Try to connect to each newly detected serial port
        for port in available_ports:
            if port not in active_coms:
                try:
                    chosen_com.set(port)
                    toggle_serial_connection()
                    print(f"Connected to {port}")
                    break
                except serial.SerialException as e:
                    print(f"Failed to connect to {port}: {e}")
    # Update the list of known active COM ports
    active_coms[:] = available_ports
    # Execute auto-connect check every 1 seconds
    root.after(1000, auto_connect)


def trace_variables(*args):
    if ser.is_open:
        # Check all variables to make sure they all have values
        if (amplitudeVar.get() or peakToPeakVar.get()) and all([entryFP.get(), offsetVar.get()]):
            synchronise()


def switch(*args):
    if ser.is_open:
        synchronise()

# Adding Tracking to Variables when changed
amplitudeVar.trace_add("write", trace_variables)
peakToPeakVar.trace_add("write", trace_variables)
offsetVar.trace_add("write", trace_variables)
lb.bind("<<ListboxSelect>>", trace_variables)
waveStateVar.trace_add("write", switch)


def main():
    print("Initialising UART")
    # Initialise the uart with the selected COM from the GUI
    uart_init(chosen_com.get())
    threading.Thread(target=read_from_port, args=(ser,), daemon=True).start()
    auto_connect()
    update_fp_unit()
    update_ap_unit()
    send_periodic_data()
    root.mainloop()


if __name__ == '__main__':
    main()
