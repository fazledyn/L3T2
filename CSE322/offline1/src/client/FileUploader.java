package client;

import server._CONFIG_;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;

public class FileUploader implements Runnable {
    
    private final Socket fileSocket;
    private final Socket textSocket;
    private final DataOutputStream dos;
    private final DataInputStream dis;
    
    private final byte[] fileContent;
    
    FileUploader(Socket textSocket, Socket fileSocket, byte[] fileContent) throws Exception {
        this.fileSocket = fileSocket;
        this.textSocket = textSocket;
        this.dos = new DataOutputStream(fileSocket.getOutputStream());
        this.dis = new DataInputStream(fileSocket.getInputStream());
        this.fileContent = fileContent;
    }
    
    String byteToString(InputStream is) throws Exception {
        byte[] buffer = new byte[is.available()];
        is.read(buffer, 0, buffer.length);
        return new String(buffer);
    }
    
    @Override
    public void run() {
        
        while (fileSocket.isConnected() && textSocket.isConnected()) {
            try {
                fileSocket.setSoTimeout(_CONFIG_.SOCKET_TIMEOUT);
                while (dis.available() < 1) ;
                int chunkSize = dis.readInt();
                
                ArrayList<byte[]> chunkList = splitFile(chunkSize);
                
                for (byte[] bytes : chunkList) {
                    dos.write(bytes);
                    dos.flush();
                    
                    while (dis.available() < 1) ;
                    if (!dis.readUTF().equals("ACK")) {
                        return;
                    }
                }
                fileSocket.setSoTimeout(0);
                return;
            }
            catch (Exception e) {
                e.printStackTrace();
                break;
            }
        }
        
    }
    
    ArrayList<byte[]> splitFile(int chunkSize) {
        int start = 0;
        int fileSize = fileContent.length;
        ArrayList<byte[]> chunkList = new ArrayList<>();
        
        while (start < fileSize) {
            int end = Math.min(fileSize, start + chunkSize);
            chunkList.add(Arrays.copyOfRange(fileContent, start, end));
            start += chunkSize;
        }
        return chunkList;
    }
    
}
