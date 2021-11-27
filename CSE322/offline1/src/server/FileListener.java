package server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;


public class FileListener implements Runnable {
    
    private final Socket fileSocket;
    private final Socket textSocket;
    
    private final String fileName;
    private final String fileType;
    private final int fileSize;
    
    private final String studentId;
    private final int requestId;
    
    FileListener(Socket fileSocket,
                 Socket textSocket,
                 String studentId,
                 String fileName,
                 String fileType,
                 int fileSize,
                 int requestId) {
        this.studentId = studentId;
        this.fileName = fileName;
        this.fileType = fileType;
        this.fileSize = fileSize;
        
        this.fileSocket = fileSocket;
        this.textSocket = textSocket;
        
        this.requestId = requestId;
    }
    
    @Override
    public void run() {
        
        while (textSocket.isConnected() && fileSocket.isConnected()) {
            if (Database.get().fileQueue.size() > 0) {
                if (Database.get().fileQueue.peek().equals(studentId)) {
                    if (!Database.get().isBufferOverflow(fileSize)) {
                        
                        Database.get().fileQueue.poll();
                        Database.get().setBuffer(Database.get().getBuffer() + fileSize);
                        
                        try {
                            fileSocket.setSoTimeout(30000);
                            
                            DataOutputStream dos = new DataOutputStream(textSocket.getOutputStream());
                            DataOutputStream fos = new DataOutputStream(fileSocket.getOutputStream());
                            DataInputStream fis = new DataInputStream(fileSocket.getInputStream());
                            
                            ArrayList<byte[]> chunkList = new ArrayList<>();
                            int chunkSize = ThreadLocalRandom.current().nextInt(_CONFIG_.MIN_CHUNK_SIZE, _CONFIG_.MAX_CHUNK_SIZE + 1);
                            int incomingSize = 0;
                            
                            fos.writeInt(chunkSize);
                            fos.flush();
                            
                            int nChunks = (int) Math.ceil((double) fileSize / chunkSize);
                            
                            for (int i = 0; i < nChunks; i++) {
                                while (fis.available() < 1) ;
                                
                                byte[] incoming = new byte[fis.available()];
                                fis.read(incoming, 0, incoming.length);
                                chunkList.add(incoming);
                                incomingSize += incoming.length;
                                
                                fos.writeUTF("ACK");
                                fos.flush();
                            }
                            
                            if (incomingSize != fileSize) {
                                dos.writeUTF("File transmission is corrupted");
                                dos.flush();
                                return;
                            }
                            
                            int id;
                            if (requestId == -1) {
                                id = Database.get().saveAsFile(chunkList, fileName, fileType, studentId);
                            }
                            else {
                                id = Database.get().saveAsRequestFile(chunkList, fileName, studentId, requestId);
                            }
                            dos.writeUTF("File has been uploaded. FileID #" + id);
                            dos.flush();
                            
                        }
                        catch (Exception e) {
                            e.printStackTrace();
                            Database.get().setBuffer(Database.get().getBuffer() - fileSize);
                            break;
                        }
                        Database.get().setBuffer(Database.get().getBuffer() - fileSize);
                        return;
                    }
                }
            }
        }
        
    }
}
