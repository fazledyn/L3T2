package client;

import java.io.*;
import java.net.*;

public class FileDownloader implements Runnable {

    private final Socket textSocket;
    private final Socket fileSocket;
    private final String fileName;
    private final InputStream is;

    FileDownloader(Socket textSocket, Socket fileSocket, String fileName) throws Exception {
        this.textSocket = textSocket;
        this.fileSocket = fileSocket;
        this.fileName = fileName;
        this.is = fileSocket.getInputStream();
    }

    @Override
    public void run() {
    
        new File("downloads/").mkdirs();

        while (fileSocket.isConnected() && textSocket.isConnected()) {
            try {
                FileOutputStream fstream = new FileOutputStream("downloads/" + fileName, true);
                while (is.available() < 1);
                
                while (is.available() > 0) {
                    byte[] buffer = new byte[is.available()];
                    is.read(buffer);
                    fstream.write(buffer);
                }
                fstream.close();
                break;
            }
            catch (Exception e) {
                e.printStackTrace();
                break;
            }
        }
    }


}
