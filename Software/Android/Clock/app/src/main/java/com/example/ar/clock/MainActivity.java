package com.example.ar.clock;

import android.content.Context;
import android.content.DialogInterface;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TimePicker;

import com.felhr.usbserial.UsbSerialDevice;

import java.util.HashMap;
import java.util.Map;

public class MainActivity extends AppCompatActivity
{
    /** The time picker user interface object. */
    private TimePicker _timePicker;

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
     * @return The first serial cable found.
     */
    private UsbSerialDevice findSerialDevice()
    {
        // Retrieve all connected USB devices
        UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> usbDevicesMap = usbManager.getDeviceList();
        if (usbDevicesMap.isEmpty()) return null;

        // Is one of this devices a compatible serial cable ?
        UsbDeviceConnection usbDeviceConnection;
        for(Map.Entry<String, UsbDevice> usbDevicesMapEntry : usbDevicesMap.entrySet())
        {
            UsbDevice usbDevice = usbDevicesMapEntry.getValue();

            // Connect to the device
            usbDeviceConnection = usbManager.openDevice(usbDevice);

            // Is it a compatible serial cable ?
            UsbSerialDevice usbSerialDevice = UsbSerialDevice.createUsbSerialDevice(usbDevice, usbDeviceConnection);
            if (usbSerialDevice != null) return usbSerialDevice;
        }

        // No compatible device was found
        return null;
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
    }

    /** Called when the button is pressed. */
    public void buttonClick(View view)
    {
        // Try to connect to the serial cable
        UsbSerialDevice serialDevice = findSerialDevice();
        if (serialDevice == null)
        {
            displayMessage("Error", "No compatible serial cable was detected.");
            return;
        }

        // Get alarm time
        displayMessage("bouh", "test");
    }
}
