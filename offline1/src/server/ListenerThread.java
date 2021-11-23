package server;

import java.io.*;
import java.net.*;
import java.util.concurrent.ThreadLocalRandom;

public class ListenerThread implements Runnable {

    private final DataOutputStream dos;
    private final DataInputStream dis;
    private final Socket socket;
    private final String studentId;

    ListenerThread(Socket socket, DataOutputStream dos, DataInputStream dis, String studentId) {
        this.dos = dos;
        this.dis = dis;
        this.socket = socket;
        this.studentId = studentId;
    }

    @Override
    public void run() {
        while (!socket.isClosed()) {
            try {
                executeCommand(dis.readUTF());
            }
            catch (Exception e) {
                e.printStackTrace(System.err);
            }
        }
    }

    void executeCommand(String command) throws Exception {
        String[] commandList = command.split(" ");
        switch (commandList[0]) {

            case "help" -> {
                dos.writeUTF("*** Available Commands ***");
                dos.writeUTF("**************************");
                dos.writeUTF("1. students");
                dos.writeUTF("2. messages");
                dos.writeUTF("3. files self");
                dos.writeUTF("4. files [STUDENT_ID]");
                dos.writeUTF("5. request");
                dos.writeUTF("6. logout");
                dos.writeUTF("7. upload");
                dos.flush();
            }

            case "logout" -> {
                Database.getInstance().logoutStudent(studentId);
                dos.writeUTF("You have logged out.");
                dos.writeUTF("TERMINATE");
                dos.flush();
                socket.close();
            }

            case "students" -> {
                dos.writeUTF("*** Available Student List ***");
                dos.writeUTF("******************************");
                dos.writeUTF("Student ID\t\tStatus");
                dos.flush();
                for (String each : Database.getInstance().getStudentList(studentId)) {
                    dos.writeUTF(each);
                    dos.flush();
                }
            }

            case "messages" -> {
                if (Database.getInstance().getMessages(studentId).size() == 0) {
                    dos.writeUTF("You have no unread messages");
                    dos.flush();
                    break;
                }
                dos.writeUTF("*** Unread Messages: ***");
                dos.writeUTF("************************");
                dos.flush();
                for (String each : Database.getInstance().getMessages(studentId)) {
                    dos.writeUTF(each);
                    dos.flush();
                }
                Database.getInstance().removeMessages(studentId);
            }

            case "files" -> {
                if (commandList.length < 2) break;

                if (commandList[1].equals("self")) {
                    if (Database.getInstance().getFilesList(studentId, studentId).size() == 0) {
                        dos.writeUTF("You have no files");
                        dos.flush();
                        break;
                    }
                    dos.writeUTF("Your Files");
                    dos.writeUTF("**********");
                    dos.writeUTF("FileID\t\tFileName\t\tStatus");

                    for (String each : Database.getInstance().getFilesList(studentId, studentId)) {
                        dos.writeUTF(each);
                        dos.flush();
                    }
                }
                else {
                    if (Database.getInstance().getFilesList(studentId, commandList[1]).size() == 0) {
                        dos.writeUTF("Student '" + commandList[1] + "' has no files");
                        dos.flush();
                        break;
                    }
                    dos.writeUTF("Student '" + commandList[1] + "'s files:");
                    dos.writeUTF("****************************");
                    dos.writeUTF("FileID\t\tFileName\t\tStatus");

                    for (String each : Database.getInstance().getFilesList(studentId, studentId)) {
                        dos.writeUTF(each);
                        dos.flush();
                    }
                }
            }

            case "request" -> {
                dos.writeUTF("Enter file description: ");
                dos.flush();

                // *** We create a request, obtain the request ID and then broadcast it to everyone.
                String description = dis.readUTF();
                int requestId = Database.getInstance().addRequest(studentId, description);
                String message= "Student `" + studentId + "` has requested a file.\n" +
                                "Request ID: " + requestId + "\n" +
                                "File Description: \n" + description + "\n";
                Database.getInstance().addBroadcastMessage(studentId, message);

                dos.writeUTF("The request has been submitted and broadcast to other students");
                dos.flush();
            }

            case "upload" -> {
                int chunkSize = ThreadLocalRandom.current().nextInt(CONFIG.MIN_CHUNK_SIZE, CONFIG.MAX_CHUNK_SIZE + 1);
                dos.writeUTF("Random chunk size is: " + chunkSize);
                dos.flush();
            }

            default -> {
                dos.writeUTF("Invalid command. Please enter 'help' to view commands.");
                dos.flush();

            }

        }
    }
}


