package client;

import java.io.DataInputStream;
import java.net.Socket;

public class ReceiverThread implements Runnable {
    
    public static final String ANSI_RESET = "\u001B[0m";
    public static final String ANSI_CYAN = "\u001B[36m";
    
    private final Socket fileSocket;
    private final Socket textSocket;
    private final DataInputStream tis;
    
    ReceiverThread(Socket textSocket, Socket fileSocket, DataInputStream tis) {
        this.textSocket = textSocket;
        this.fileSocket = fileSocket;
        this.tis = tis;
    }
    
    @Override
    public void run() {
        while (!textSocket.isClosed()) {
            try {
                String output = tis.readUTF();
                if (!output.equals("TERMINATE")) {
                    System.out.println(ANSI_CYAN + output + ANSI_RESET);
                }
                else {
                    textSocket.close();
                    fileSocket.close();
                    System.exit(0);
                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
