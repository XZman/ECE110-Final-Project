package com.placeholder.ece110finalcar;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;

import java.util.Arrays;

import io.github.controlwear.virtual.joystick.android.JoystickView;

public class MainActivity extends AppCompatActivity {

    final static byte[] bytesData = new byte[] {(byte) -128, (byte) 127};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final JoystickView joystick = findViewById(R.id.joystick);
        joystick.setOnMoveListener(new JoystickView.OnMoveListener() {
            @Override
            public void onMove(int angle, int strength) {
                int leftSpeed = MainActivity.computeLeftSpeed(angle, strength);
                int rightSpeed = MainActivity.computeRightSpeed(angle, strength);
                Log.i("speeds", leftSpeed + "," + rightSpeed);
            }
        });

        // start receiving packets
        Thread tr = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    try {
                        byte[] data = new byte[10000];
                        final byte[] rdata = UDPClient.receiveDatagram(23333, data);
                        Log.i("received", new String(rdata));
                    }catch (Exception e) {
                        Log.e("error receive", e.getMessage(), e.getCause());
                    }
                }
            }
        });
        //tr.start();

        Thread ts = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    try {
                        byte[] speedData = new byte[] {(byte) getLeftSpeed(), (byte) getRightSpeed()};
                        UDPClient.sendDatagram("192.168.43.16",11122, speedData, 10);
                        Log.i("sent", Arrays.toString(speedData));
                        Thread.sleep(200);
                    } catch (Exception e) {
                        Log.e("send error", e.getMessage(), e.getCause());
                    }
                }
            }
        });
        ts.start();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private static volatile int leftSpeed = 0;
    private static volatile int rightSpeed = 0;

    public static int getLeftSpeed() {
        return leftSpeed;
    }

    public static int getRightSpeed() {
        return rightSpeed;
    }

    public static synchronized int computeLeftSpeed(int angle, int strength) {
        float speed = 0;

        if (0 <= angle && angle < 90)
            speed = 100f;
        if (90 <= angle && angle < 180)
            speed = -10.0f / 9 * angle + 200;
        if (180 <= angle && angle < 270)
            speed = -10.0f / 9 * angle + 200;
        if (270 <= angle && angle < 360)
            speed = -100f;

        speed *= (float) strength / 100;

        leftSpeed = (int) speed;

        return (int) speed;
    }

    public static synchronized int computeRightSpeed(int angle, int strength) {
        float speed = 0;

        if (0 <= angle && angle < 90)
            speed = 10f / 9 * angle;
        if (90 <= angle && angle < 180)
            speed = 100f;
        if (180 <= angle && angle < 270)
            speed = -100f;
        if (270 <= angle && angle < 360)
            speed = 10f / 9 * angle - 400;

        speed *= (float) strength / 100;

        rightSpeed = (int) speed;

        return (int) speed;
    }
}
