package server;

import java.net.*;

public class Server {

    private ServerSocket ss;
    private ServerSocket fs;

    Server() {
        try {
            ss = new ServerSocket(6666);
            fs = new ServerSocket(6667);
        }
        catch (Exception e) {
            System.err.println("PROBLEM > In creating server sockets");
            e.printStackTrace(System.err);
        }
    }

    void start() {
        while (!ss.isClosed()) {
            try {
                Socket socket = ss.accept();
                Socket fSocket = fs.accept();
                System.out.println(socket.toString());

                Thread thread = new Thread(new ClientHandlerThread(socket));
                thread.start();
            }
            catch (Exception e) {
                System.err.println("Error in accepting socket from client");
            }
        }
    }

}
