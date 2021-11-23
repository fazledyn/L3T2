package server;

import java.io.*;
import java.net.*;

public class ClientHandlerThread implements Runnable {

    private Socket socket;
    private DataOutputStream dos;
    private DataInputStream dis;

    ClientHandlerThread(Socket socket) {
        try {
            this.socket = socket;
            this.dos = new DataOutputStream(socket.getOutputStream());
            this.dis = new DataInputStream(socket.getInputStream());
        }
        catch (Exception e) {
            System.err.println("Error in creating IO stream");
        }
    }

    boolean authenticate(String studentId) {
        if (Database.getInstance().isStudentOnline(studentId)) return false;

        Database.getInstance().addStudent(studentId, this.socket);
        return true;
    }

    @Override
    public void run() {
        try {
            dos.writeUTF("Enter student id: ");
            dos.flush();
            String studentId = dis.readUTF();

            // ** If the client isn't authenticated, we terminate the connection
            if (!authenticate(studentId)) {
                dos.writeUTF("You are already logged in another session.\nConnection Terminated");
                dos.writeUTF("TERMINATE");
                dos.flush();
                socket.setKeepAlive(false);
                return;
            }
            // **** Creating directory for `studentId`
            new File("files/" + studentId + "/private").mkdirs();
            new File("files/" + studentId + "/public").mkdirs();

            dos.writeUTF("Welcome, " + studentId);
            dos.writeUTF("Enter 'help' to show different commands.");
            dos.flush();
            new Thread(new ListenerThread(this.socket, this.dos, this.dis, studentId)).start();
        }
        catch (Exception e) {
            e.printStackTrace(System.err);
        }
    }

}
