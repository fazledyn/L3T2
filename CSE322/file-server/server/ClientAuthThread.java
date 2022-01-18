package server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.net.Socket;

public class ClientAuthThread implements Runnable {
    
    private Socket textSocket;
    private DataInputStream tis;
    private DataOutputStream tos;
    
    private Socket fileSocket;
    
    ClientAuthThread(Socket textSocket, Socket fileSocket) {
        try {
            this.textSocket = textSocket;
            this.tis = new DataInputStream(textSocket.getInputStream());
            this.tos = new DataOutputStream(textSocket.getOutputStream());
            
            this.fileSocket = fileSocket;
        }
        catch (Exception e) {
            System.err.println("Error in creating IO stream");
        }
    }
    
    boolean authenticate(String studentId) {
        if (Database.get().isStudentOnline(studentId)) return false;
        
        Database.get().addStudent(studentId);
        return true;
    }
    
    void createStudentDirectory(String studentId) {
        new File("files/" + studentId + "/private").mkdirs();
        new File("files/" + studentId + "/public").mkdirs();
    }
    
    @Override
    public void run() {
        try {
            tos.writeUTF("Enter student id: ");
            tos.flush();
            String studentId = tis.readUTF();
            
            // ** If the client isn't authenticated, we terminate the connection
            if (!authenticate(studentId)) {
                tos.writeUTF("You are already logged in another session.\nConnection Terminated");
                tos.writeUTF("TERMINATE");
                tos.flush();
                
                textSocket.setKeepAlive(false);
                fileSocket.setKeepAlive(false);
                return;
            }
            createStudentDirectory(studentId);
            tos.writeUTF("Welcome, " + studentId);
            tos.writeUTF("Enter `help` to show commands");
            tos.flush();
            
            Thread mainThread = new Thread(new CommandListener(studentId, textSocket, tis, tos, fileSocket));
            mainThread.start();
        }
        catch (Exception e) {
            e.printStackTrace(System.err);
        }
    }
    
}
