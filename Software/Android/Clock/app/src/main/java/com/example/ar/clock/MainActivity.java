package com.example.ar.clock;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TimePicker;

import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

public class MainActivity extends AppCompatActivity
{
    /** The time picker user interface object. */
    private TimePicker _timePicker;

    /** The USB device corresponding to the USB cable. */
    private UsbDevice _usbDevice;
    /** The serial cable USB device. */
    private UsbSerialDevice _usbSerialDevice;

    /** The UART protocol magic number (a value that could hardly be randomly generated on the bus). */
    private final byte _COMMUNICATION_PROTOCOL_MAGIC_NUMBER = (byte) 0xA5;
    /** An UART transmission or reception timeout in milliseconds. */
    private final int _COMMUNICATION_PROTOCOL_TIMEOUT = 5000;

    /** The permission we are waiting for. */
    private final String _PERMISSION_USB_ACCESS = "com.android.example.USB_PERMISSION";
    /** Tell if the serial cable can be configured and accessed by the app or not. */
    private boolean _isSerialDeviceUsable;

    /** Display a simple dialog window waiting for the user to hit the "ok" button.
     * @param title The dialog title.
     * @param message The dialog message.
     */
    private void displayMessage(CharSequence title, CharSequence message)
    {
        // Create a dummy listener just to add a button to the dialog
        DialogInterface.OnClickListener buttonClickListener = new DialogInterface.OnClickListener()
        {
            public void onClick(DialogInterface dialog, int id) {}
        };

        // Prepare the dialog content
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(MainActivity.this);
        dialogBuilder.setTitle(title);
        dialogBuilder.setMessage(message);
        dialogBuilder.setNeutralButton("OK", buttonClickListener);

        // Display the dialog
        AlertDialog dialog = dialogBuilder.create();
        dialog.show();
    }

    /** Probe the USB bus to find a working serial cable
     * @return 0 if the serial device was successfully found,
     * @return -1 if no compatible device was found.
     */
    private int findSerialDevice()
    {
        // Retrieve all connected USB devices
        UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> usbDevicesMap = usbManager.getDeviceList();
        if (usbDevicesMap.isEmpty()) return -1;

        // Is one of this devices a compatible serial cable ?
        UsbDeviceConnection usbDeviceConnection;
        for(Map.Entry<String, UsbDevice> usbDevicesMapEntry : usbDevicesMap.entrySet())
        {
            _usbDevice = usbDevicesMapEntry.getValue();

            // Connect to the device
            usbDeviceConnection = usbManager.openDevice(_usbDevice);

            // Is it a compatible serial cable ?
            _usbSerialDevice = UsbSerialDevice.createUsbSerialDevice(_usbDevice, usbDeviceConnection);
            if (_usbSerialDevice != null) return 0;
        }

        // No compatible device was found
        return -1;
    }

    /** Receive the USB permission access result. */
    private final BroadcastReceiver _usbDeviceBroadcastReceiver = new BroadcastReceiver()
    {
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            if (action.equals(_PERMISSION_USB_ACCESS))
            {
                synchronized (this)
                {
                    UsbDevice device = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if ((intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) && (device != null)) _isSerialDeviceUsable = true;
                    else _isSerialDeviceUsable = false;
                }
            }
        }
    };

    /** Set the UART communication baud rate, stop bits amount...
     * @return 0 if the serial device was successfully configured,
     * @return -1 if an error occurred.
     */
    private int configureSerialDevice()
    {
        // Ask the permission to access to the device
        UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        PendingIntent permissionPendingIntent = PendingIntent.getBroadcast(this, 0, new Intent(_PERMISSION_USB_ACCESS), 0);
        usbManager.requestPermission(_usbDevice, permissionPendingIntent);
        if (!_isSerialDeviceUsable) return -1;

        // Try to gain access to the cable
        if (!_usbSerialDevice.syncOpen()) return -1;

        // Set communication parameters
        _usbSerialDevice.setBaudRate(19200);
        _usbSerialDevice.setDataBits(UsbSerialInterface.DATA_BITS_8);
        _usbSerialDevice.setStopBits(UsbSerialInterface.STOP_BITS_1);
        _usbSerialDevice.setParity(UsbSerialInterface.PARITY_NONE);
        _usbSerialDevice.setFlowControl(UsbSerialInterface.FLOW_CONTROL_OFF);

        return 0;
    }

    /** Write a byte of data on the UART.
     * @param data The byte to send.
     */
    private void sendByte(byte data)
    {
        // Prepare the buffer to send
        byte dataBuffer[] = new byte[1];
        dataBuffer[0] = data;

        // Try to send a byte
        if (_usbSerialDevice.syncWrite(dataBuffer, _COMMUNICATION_PROTOCOL_TIMEOUT) != 1) displayMessage("Error", "Failed to send a byte of data.");
    }

    /** Convert a byte to a couple of Binary Coded Decimal values and transmit it over the UART. The byte value must be in range 0 to 99.
     * @param data The value to send.
     */
    private void sendBCDValue(int data)
    {
        int tens, units;
        byte bcdData;

        // Convert the binary data to Binary Coded Decimal
        tens = data / 10;
        units = data - (tens * 10);
        bcdData = (byte) ((tens << 4) | units);

        // Transmit the converted data
        sendByte(bcdData);
    }

    /** Wait for a byte to be received from the UART.
     * @return The received byte (an error message is displayed if the timeout fired).
     */
    private byte receiveByte()
    {
        byte dataBuffer[] = new byte[1];

        // Try to receive a byte
        if (_usbSerialDevice.syncRead(dataBuffer, _COMMUNICATION_PROTOCOL_TIMEOUT) != 1) displayMessage("Error", "Failed to receive a byte of data.");

        return dataBuffer[0];
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Get access to the user interface objects
        _timePicker = (TimePicker) findViewById(R.id.timePicker);

        // Force time picker to be in 24 hours mode
        _timePicker.setIs24HourView(true);

        // Register the callback object called when the USB access permission has been granted or denied by the user
        IntentFilter filter = new IntentFilter(_PERMISSION_USB_ACCESS);
        registerReceiver(_usbDeviceBroadcastReceiver, filter);
    }

    @Override
    protected void onDestroy()
    {
        unregisterReceiver(_usbDeviceBroadcastReceiver);
    }

    /** Called when the button is pressed. */
    public void buttonClick(View view)
    {
        // Try to connect to the serial cable
        if (findSerialDevice() != 0)
        {
            displayMessage("Error", "No compatible serial cable was detected.");
            return;
        }

        // Set the serial communication settings
        if (configureSerialDevice() != 0)
        {
            displayMessage("Error", "Failed to configure the serial communication settings.");
            return;
        }

        // Connect to the clock
        sendByte(_COMMUNICATION_PROTOCOL_MAGIC_NUMBER);
        if (receiveByte() != _COMMUNICATION_PROTOCOL_MAGIC_NUMBER)
        {
            displayMessage("Error", "Failed to connect to the clock.");
            _usbSerialDevice.syncClose();
            return;
        }

        // Send time and date now that all time consuming operations are done
        Calendar calendar = Calendar.getInstance();
        sendBCDValue(calendar.get(Calendar.SECOND));
        sendBCDValue(calendar.get(Calendar.MINUTE));
        sendBCDValue(calendar.get(Calendar.HOUR_OF_DAY));
        sendBCDValue(calendar.get(Calendar.DAY_OF_WEEK));
        sendBCDValue(calendar.get(Calendar.DAY_OF_MONTH));
        sendBCDValue(calendar.get(Calendar.MONTH) + 1); // January starts from 0
        sendBCDValue(calendar.get(Calendar.YEAR) - 2000); // RTC year starts from 2000

        // Get alarm time
        sendBCDValue(_timePicker.getCurrentHour());
        sendBCDValue(_timePicker.getCurrentMinute());

        // Wait for the clock answer
        if (receiveByte() != _COMMUNICATION_PROTOCOL_MAGIC_NUMBER)
        {
            displayMessage("Error", "Clock configuration failed.");
            _usbSerialDevice.syncClose();
            return;
        }

        displayMessage("Information", "Time, date and alarm were successfully set.");
        _usbSerialDevice.syncClose();
    }
}
