package server;

import java.io.*;
import java.net.*;
import java.util.*;

public class FileServer implements Runnable {

    private final Socket textSocket;
    private final Socket fileSocket;
    private final byte[] fileContent;
    private final OutputStream os;

    FileServer(Socket textSocket, Socket fileSocket, byte[] fileContent) throws Exception {
        this.textSocket = textSocket;
        this.fileSocket = fileSocket;
        this.fileContent = fileContent;
        this.os = fileSocket.getOutputStream();
    }

    @Override
    public void run() {

        while (fileSocket.isConnected() && textSocket.isConnected()) {
            try {
                os.write(fileContent);
                os.flush();
                break;
            }
            catch (Exception e) {
                e.printStackTrace();
                break;
            }
        }

    }

    ArrayList<byte[]> splitFile() throws IOException {

        int start = 0;
        int fileSize = fileContent.length;
        ArrayList<byte[]> chunkList = new ArrayList<>();

        while (start < fileSize) {
            int end = Math.min(fileSize, start + _CONFIG_.MAX_CHUNK_SIZE);
            chunkList.add(Arrays.copyOfRange(fileContent, start, end));
            start += _CONFIG_.MAX_CHUNK_SIZE;
        }
        return chunkList;
    }

}
