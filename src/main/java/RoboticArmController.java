import com.fazecast.jSerialComm.SerialPort;

public class RoboticArmController {

    private SerialPort serialPort;
    private static final int BAUD_RATE = 115200;

    // Kinematics Constants
    public static final double BASE_STEPS_PER_DEGREE = 168.89;
    public static final double SHOULDER_STEPS_PER_DEGREE = 37.78;
    public static final double STEPS_PER_MM = 10.0; // Placeholder

    public RoboticArmController(String portName) throws Exception {
        serialPort = SerialPort.getCommPort(portName);
        serialPort.setBaudRate(BAUD_RATE);

        if (!serialPort.openPort()) {
            throw new Exception("Failed to open serial port: " + portName);
        }

        serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 10000, 0);

        // Allow some time for Arduino to reset after connecting
        Thread.sleep(2000);

        // Send power management command to enable/lock motors
        sendCommand("E1\n");
    }

    private void sendCommand(String command) {
        byte[] bytes = command.getBytes();
        serialPort.writeBytes(bytes, bytes.length);
    }

    public void syncMove(long basePos, long shoulderPos, long lPos, long ePos, long uPos, long rPos) throws Exception {
        String command = String.format("B%d S%d L%d E%d U%d R%d\n", basePos, shoulderPos, lPos, ePos, uPos, rPos);
        sendCommand(command);

        StringBuilder response = new StringBuilder();
        long startTime = System.currentTimeMillis();
        long timeout = 10000; // 10 seconds

        while (true) {
            if (System.currentTimeMillis() - startTime > timeout) {
                throw new Exception("Timeout waiting for READY response from Arduino");
            }

            if (serialPort.bytesAvailable() > 0) {
                byte[] readBuffer = new byte[serialPort.bytesAvailable()];
                int numRead = serialPort.readBytes(readBuffer, readBuffer.length);

                if (numRead > 0) {
                    response.append(new String(readBuffer));
                    if (response.toString().contains("READY\n")) {
                        break;
                    }
                }
            } else {
                Thread.sleep(50); // Small delay to prevent tight loop
            }
        }
    }

    public void shutdown() {
        if (serialPort != null && serialPort.isOpen()) {
            sendCommand("E0\n");

            // Allow a brief moment for the command to transmit before closing
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }

            serialPort.closePort();
        }
    }

    // Vision Integration Stub
    public long[] calculateTargetFromVision(double x, double y) {
        // Placeholder for calculating base and slider steps from vision coordinates
        long sliderSteps = Math.round(y * STEPS_PER_MM);
        long baseSteps = Math.round((Math.atan2(y, x) * 180 / Math.PI) * BASE_STEPS_PER_DEGREE);

        return new long[]{baseSteps, sliderSteps};
    }

    // Main method for simple testing (Optional)
    public static void main(String[] args) {
        System.out.println("RoboticArmController ready to build.");
    }
}
