package server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.net.Socket;

public class CommandListener implements Runnable {
    
    private final String studentId;
    
    private final Socket textSocket;
    private final DataInputStream tis;
    private final DataOutputStream tos;
    
    private final Socket fileSocket;
    
    public CommandListener(String studentId,
    
                           Socket textSocket,
                           DataInputStream tis,
                           DataOutputStream tos,
    
                           Socket fileSocket) {
        this.studentId = studentId;
        this.textSocket = textSocket;
        this.tis = tis;
        this.tos = tos;
        this.fileSocket = fileSocket;
    }
    
    @Override
    public void run() {
        while (!textSocket.isClosed()) {
            try {
                executeCommand(tis.readUTF());
            }
            catch (Exception e) {
                e.printStackTrace(System.err);
                break;
            }
        }
    }
    
    void executeCommand(String command) throws Exception {
        String[] commandList = command.split(" ");
        switch (commandList[0]) {
            
            case "help" -> {
                tos.writeUTF("*** Available Commands ***");
                tos.writeUTF("**************************");
                tos.writeUTF("> students");
                tos.writeUTF("> messages");
                tos.writeUTF("> files self");
                tos.writeUTF("> files [student id]");
                tos.writeUTF("> request");
                tos.writeUTF("> upload [private/public] [filepath] [request id]");
                tos.writeUTF("> download [file id]");
                tos.writeUTF("> logout");
                tos.flush();
            }
            
            case "logout" -> {
                Database.get().logoutStudent(studentId);
                tos.writeUTF("You have logged out.");
                tos.writeUTF("TERMINATE");
                tos.flush();
                
                textSocket.close();
                fileSocket.close();
            }
            
            case "students" -> {
                tos.writeUTF("*** Available Student List ***");
                tos.writeUTF("******************************");
                tos.writeUTF("Student ID\t\tStatus");
                tos.flush();
                for (String each : Database.get().getStudentList(studentId)) {
                    tos.writeUTF(each);
                    tos.flush();
                }
            }
            
            case "messages" -> {
                if (Database.get().getMessages(studentId).size() == 0) {
                    tos.writeUTF("You have no unread messages");
                    tos.flush();
                    break;
                }
                tos.writeUTF("*** Unread Messages: ***");
                tos.writeUTF("******************************");
                tos.flush();
                for (String each : Database.get().getMessages(studentId)) {
                    tos.writeUTF(each);
                    tos.flush();
                }
                Database.get().removeMessages(studentId);
            }
            
            case "files" -> {
                if (commandList.length < 2) break;
                
                if (commandList[1].equals("self")) {
                    if (Database.get().getFilesList(studentId, studentId).size() == 0) {
                        tos.writeUTF("You have no files");
                        tos.flush();
                        break;
                    }
                    tos.writeUTF("Your Files");
                    tos.writeUTF("******************************");
                    tos.writeUTF("FileID\t\tFileName\t\tStatus");
                    
                    for (String each : Database.get().getFilesList(studentId, studentId)) {
                        tos.writeUTF(each);
                        tos.flush();
                    }
                }
                else {
                    if (Database.get().getFilesList(studentId, commandList[1]).size() == 0) {
                        tos.writeUTF("Student '" + commandList[1] + "' has no files");
                        tos.flush();
                        break;
                    }
                    tos.writeUTF("Student '" + commandList[1] + "'s files:");
                    tos.writeUTF("******************************");
                    tos.writeUTF("FileID\t\tFileName\t\tStatus");
                    
                    for (String each : Database.get().getFilesList(studentId, commandList[1])) {
                        tos.writeUTF(each);
                        tos.flush();
                    }
                }
            }
            
            case "request" -> {
                tos.writeUTF("Enter file description: ");
                tos.flush();
                
                // *** We create a request, obtain the request ID and then broadcast it to everyone.
                String description = tis.readUTF();
                int requestId = Database.get().addRequest(studentId, description);
                String message = "Student `" + studentId + "` has requested a file.\n" +
                        "Request ID: " + requestId + "\n" +
                        "File Description: \n" + description + "\n";
                Database.get().addBroadcastMessage(studentId, message);
                
                tos.writeUTF("The request has been submitted and broadcast to other students");
                tos.flush();
            }
            
            case "upload" -> {
                int requestId = -1;
                
                String fileType = commandList[1];
                String fileName = commandList[2];
                int fileSize = Integer.parseInt(commandList[3]);
                
                if (fileSize > _CONFIG_.MAX_BUFFER_SIZE) {
                    tos.writeUTF("The file size is bigger than the buffer size.");
                    tos.flush();
                    break;
                }
                
                if (commandList.length == 5) {
                    try {
                        requestId = Integer.parseInt(commandList[4]);
                    }
                    catch (Exception e) {
                        tos.writeUTF("Invalid file request ID.");
                        break;
                    }
                }
                
                Database.get().fileQueue.add(studentId);
                Thread fileListenerThread = new Thread(new FileListener(fileSocket, textSocket, studentId, fileName, fileType, fileSize, requestId));
                fileListenerThread.start();
            }
            
            case "download" -> {
                
                int fileId;
                if (commandList.length < 2) {
                    tos.writeUTF("Please specify fileID for downloading.");
                    tos.flush();
                    break;
                }
                
                try {
                    fileId = Integer.parseInt(commandList[1]);
                    Database.get().getFile(fileId, studentId);
                }
                catch (Exception e) {
                    tos.writeUTF("Invalid fileID given.");
                    tos.flush();
                    break;
                }
                
                File file = Database.get().getFile(fileId, studentId);
                
                if (file == null) {
                    tos.writeUTF("You have entered invalid fileID");
                    tos.flush();
                    break;
                }
                FileInputStream fstream = new FileInputStream(file);
                
                Thread fileServerThread = new Thread(new FileServer(textSocket, fileSocket, fstream.readAllBytes()));
                fileServerThread.start();
            }
            
            default -> {
                tos.writeUTF("Invalid command. Please enter 'help' to view commands.");
                tos.flush();
            }
            
        }
    }
}


