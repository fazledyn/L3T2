package client;

import java.io.*;
import java.net.*;

public class ReceiverThread implements Runnable {

    public static final String ANSI_RESET = "\u001B[0m";
    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_CYAN = "\u001B[36m";

    private final DataInputStream dis;
    private final Socket socket;

    ReceiverThread(Socket socket, DataInputStream dis) {
        this.dis = dis;
        this.socket = socket;
    }

    @Override
    public void run() {
        while (!socket.isClosed()) {
            try {
                String output = dis.readUTF();
                if (!output.equals("TERMINATE")) {
                    System.out.println(ANSI_CYAN + output + ANSI_RESET);
                }
                else {
                    socket.close();
                    System.exit(0);
                }
            }
            catch (Exception e) {
                System.err.println(e);
            }
        }
    }
}
