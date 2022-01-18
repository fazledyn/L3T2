package client;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    
    private Scanner scanner;
    
    private Socket textSocket;
    private DataInputStream tis;
    private DataOutputStream tos;
    
    private Socket fileSocket;
    
    Client(int textSocketPort, int fileSocketPort) {
        try {
            this.scanner = new Scanner(System.in);
            
            this.textSocket = new Socket("localhost", textSocketPort);
            this.tis = new DataInputStream(textSocket.getInputStream());
            this.tos = new DataOutputStream(textSocket.getOutputStream());
            
            this.fileSocket = new Socket("localhost", fileSocketPort);
        }
        catch (Exception e) {
            System.err.println("Error in creating socket");
        }
    }
    
    void start() {
        
        Thread senderThread = new Thread(new SenderThread(
                scanner,
                textSocket, tos,
                fileSocket
        ));
        Thread receiverThread = new Thread(new ReceiverThread(
                textSocket, fileSocket, tis
        ));
        
        senderThread.start();
        receiverThread.start();
    }
}
