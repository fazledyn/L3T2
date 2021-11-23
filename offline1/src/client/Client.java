package client;

import java.io.*;
import java.net.*;
import java.util.*;

public class Client {

    private Socket cmdSocket;
    private Socket fileSocket;
    private DataOutputStream dos;
    private DataInputStream dis;

    Client() {
        try {
            this.cmdSocket = new Socket("localhost", 6666);
            this.fileSocket = new Socket("localhost", 6667);
            this.dos = new DataOutputStream(this.cmdSocket.getOutputStream());
            this.dis = new DataInputStream(this.cmdSocket.getInputStream());
        }
        catch (Exception e) {
            System.err.println("Error in creating socket");
        }
    }

    void start() {
        Scanner sc = new Scanner(System.in);
        System.out.println("Socket: " + cmdSocket.toString());

        Thread senderThread = new Thread(new SenderThread(sc, cmdSocket, dos));
        Thread receiverThread = new Thread(new ReceiverThread(cmdSocket, dis));

        senderThread.start();
        receiverThread.start();
    }
}
