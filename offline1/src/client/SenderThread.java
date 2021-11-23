package client;

import java.io.*;
import java.net.*;
import java.util.*;

public class SenderThread implements Runnable {

    private final Scanner sc;
    private final Socket socket;
    private final DataOutputStream dos;

    SenderThread(Scanner sc, Socket socket, DataOutputStream dos) {
        this.sc = sc;
        this.dos = dos;
        this.socket = socket;
    }

    @Override
    public void run() {
        String input;
        while (!socket.isClosed()) {
            try {
                input = sc.nextLine();
                dos.writeUTF(input);
                dos.flush();
            }
            catch (Exception e) {
                System.err.println("Error in sending command");
            }
        }
    }
}
