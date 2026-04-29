package com.cyclosort;

import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class Main {
    private static RoboticArmController controller;

    public static void main(String[] args) {
        System.out.println("Starting CycloSort Controller...");

        // Note: Change COM port as needed
        controller = new RoboticArmController("/dev/ttyACM0");

        // Home/Reset positions initially
        controller.syncMove(0, 0, 0, 0, 0, 0);

        // Start UDP listener in a background thread
        Thread listenerThread = new Thread(() -> listenForVisionCommands());
        listenerThread.start();

        // Add shutdown hook to disable motors
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Shutting down...");
            if (controller != null) {
                controller.shutdown();
            }
        }));
    }

    private static void listenForVisionCommands() {
        try (DatagramSocket socket = new DatagramSocket(5005)) {
            System.out.println("Listening for vision commands on UDP port 5005...");
            byte[] buffer = new byte[1024];

            while (true) {
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                String data = new String(packet.getData(), 0, packet.getLength()).trim();
                System.out.println("Received: " + data);

                // Parse label,x,y
                String[] parts = data.split(",");
                if (parts.length == 3) {
                    String label = parts[0];
                    double x = Double.parseDouble(parts[1]);
                    double y = Double.parseDouble(parts[2]);

                    executeSortingSequence(label, x, y);
                } else {
                    System.err.println("Invalid format. Expected label,x,y");
                }
            }
        } catch (Exception e) {
            System.err.println("Error in UDP listener:");
            e.printStackTrace();
        }
    }

    private static void executeSortingSequence(String label, double x, double y) {
        System.out.println("Executing sorting sequence for: " + label + " at (" + x + ", " + y + ")");

        // 1. Calculate target motor steps from vision coordinates
        int[] targetSteps = controller.calculateTargetFromVision(x, y);
        int targetBase = targetSteps[0];
        int targetSlider = targetSteps[1];

        // Placeholder fixed heights/rotations for sorting bins
        int targetElbowDown = 1000;
        int targetShoulderHover = 500;

        // 2. Move to hover over the item
        controller.syncMove(targetBase, targetShoulderHover, targetSlider, 0, 0, 0);

        // 3. Move down to pick up the item (simulated pick up)
        controller.syncMove(targetBase, targetShoulderHover, targetSlider, targetElbowDown, 0, 0);

        // (Pretend to activate electromagnet or gripper here)

        // 4. Move back up
        controller.syncMove(targetBase, targetShoulderHover, targetSlider, 0, 0, 0);

        // 5. Move to the appropriate bin based on the label
        int binBase = 0;
        int binSlider = 0;
        if (label.equalsIgnoreCase("nut")) {
            binBase = -1000;
            binSlider = 2000;
        } else if (label.equalsIgnoreCase("bolt")) {
            binBase = 1000;
            binSlider = 2000;
        } else {
            // Default bin
            binBase = 0;
            binSlider = 3000;
        }
        controller.syncMove(binBase, targetShoulderHover, binSlider, 0, 0, 0);

        // 6. Move down to drop
        controller.syncMove(binBase, targetShoulderHover, binSlider, targetElbowDown, 0, 0);

        // (Pretend to deactivate electromagnet or gripper here)

        // 7. Move back up
        controller.syncMove(binBase, targetShoulderHover, binSlider, 0, 0, 0);

        // 8. Return to home
        controller.syncMove(0, 0, 0, 0, 0, 0);

        System.out.println("Sorting sequence complete.");
    }
}
