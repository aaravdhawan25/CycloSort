package com.cyclosort;

import com.fazecast.jSerialComm.SerialPort;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

public class RoboticArmController {

    private SerialPort serialPort;
    private OutputStream out;
    private InputStream in;

    // Kinematics Constants
    public static final double STEPS_PER_DEGREE_BASE = 168.89;
    public static final double STEPS_PER_DEGREE_SHOULDER = 37.78;
    // Placeholder for Slider
    public static final double STEPS_PER_MM_SLIDER = 100.0; // Assume 100 steps per mm for now

    public RoboticArmController(String portName) {
        serialPort = SerialPort.getCommPort(portName);
        serialPort.setBaudRate(115200);
        serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 10000, 0); // 10 second timeout

        if (serialPort.openPort()) {
            System.out.println("Port opened successfully.");
            out = serialPort.getOutputStream();
            in = serialPort.getInputStream();

            // Enable motors on startup
            sendCommand("E1\n");
        } else {
            System.err.println("Failed to open port.");
        }
    }

    public void shutdown() {
        if (serialPort != null && serialPort.isOpen()) {
            // Disable motors on shutdown
            sendCommand("E0\n");
            serialPort.closePort();
            System.out.println("Port closed.");
        }
    }

    private void sendCommand(String command) {
        try {
            out.write(command.getBytes());
            out.flush();
        } catch (IOException e) {
            System.err.println("Failed to send command: " + command);
            e.printStackTrace();
        }
    }

    public boolean syncMove(int b, int s, int l, int e, int u, int r) {
        // Construct the command string based on target positions
        // E.g., B100 S200 L300 E400 U500 R600\n
        String command = String.format("B%d S%d L%d E%d U%d R%d\n", b, s, l, e, u, r);

        System.out.println("Sending command: " + command.trim());
        sendCommand(command);

        // Block and Wait until the Arduino returns the exact string "READY\n"
        StringBuilder responseBuilder = new StringBuilder();
        try {
            long startTime = System.currentTimeMillis();
            while (true) {
                // Check timeout
                if (System.currentTimeMillis() - startTime > 10000) {
                    System.err.println("Timeout waiting for READY response.");
                    return false;
                }

                if (in.available() > 0) {
                    char c = (char) in.read();
                    responseBuilder.append(c);

                    // Check if we have received "READY\n"
                    if (responseBuilder.toString().endsWith("READY\n")) {
                        System.out.println("Received READY.");
                        return true;
                    }
                } else {
                    // Small sleep to prevent tight loop
                    Thread.sleep(10);
                }
            }
        } catch (IOException | InterruptedException ex) {
            System.err.println("Error reading response.");
            ex.printStackTrace();
            return false;
        }
    }

    // Vision Integration Stub
    // converts camera pixel coordinates into motor steps for the Slider and Base
    public int[] calculateTargetFromVision(double x, double y) {
        // This is a stub. Real implementation would involve calibration matrix.
        // Assuming simple linear mapping for demonstration.
        int baseSteps = (int) (x * STEPS_PER_DEGREE_BASE);
        int sliderSteps = (int) (y * STEPS_PER_MM_SLIDER);

        return new int[]{baseSteps, sliderSteps};
    }
}
