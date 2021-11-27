package server;

import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    
    private ServerSocket ssText;
    private ServerSocket ssFile;
    
    Server() {
        try {
            ssText = new ServerSocket(6666);
            ssFile = new ServerSocket(6667);
        }
        catch (Exception e) {
            System.err.println("PROBLEM > In creating server sockets");
            e.printStackTrace(System.err);
        }
    }
    
    void start() {
        while (!ssText.isClosed()) {
            try {
                Socket textSocket = ssText.accept();
                Socket fileSocket = ssFile.accept();
                
                Thread clientThread = new Thread(new ClientAuthThread(textSocket, fileSocket));
                clientThread.start();
            }
            catch (Exception e) {
                System.err.println("Error in accepting socket from client");
                break;
            }
        }
    }
    
}
