package com.example.ar.clock;

import android.content.DialogInterface;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TimePicker;

import com.felhr.usbserial.UsbSerialDevice;

public class MainActivity extends AppCompatActivity
{
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
        /*UsbDevice usbDevice;
        UsbDeviceConnection usbDeviceConnection;
        UsbSerialDevice usbSerialDevice = UsbSerialDevice.createUsbSerialDevice(usbDevice, usbDeviceConnection);*/

        // Get alarm time
        displayMessage("bouh", "test");
    }

}
