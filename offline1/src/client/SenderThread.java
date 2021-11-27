package client;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.net.Socket;
import java.util.Scanner;

public class SenderThread implements Runnable {
    
    private final Scanner sc;
    
    private final Socket textSocket;
    private final DataOutputStream tos;
    
    private final Socket fileSocket;
    
    public SenderThread(Scanner sc,
                        Socket textSocket,
                        DataOutputStream tos,
                        Socket fileSocket) {
        this.sc = sc;
        this.textSocket = textSocket;
        this.tos = tos;
        this.fileSocket = fileSocket;
    }
    
    
    @Override
    public void run() {
        String input;
        while (!textSocket.isClosed()) {
            
            try {
                input = sc.nextLine();
                String[] args = input.split(" ");
                String command = args[0];
                
                if (command.equals("upload")) {
                    String filePath = args[2];
                    
                    File file = new File(filePath);
                    FileInputStream fileInputStream = new FileInputStream(file);
                    byte[] fileContent = fileInputStream.readAllBytes();
                    fileInputStream.close();
                    
                    String fullString;
                    if (args.length == 4) {
                        //           upload   [file type]         [file name]           [file size]       [request id]
                        fullString = "upload " + args[1] + " " + file.getName() + " " + fileContent.length + " " + args[3];
                    }
                    else {
                        //           upload   [file type]        [file name]            [file size]
                        fullString = "upload " + args[1] + " " + file.getName() + " " + fileContent.length;
                    }
                    tos.writeUTF(fullString);
                    tos.flush();
                    
                    Thread t = new Thread(new FileUploader(textSocket, fileSocket, fileContent));
                    t.start();
                }
                else if (command.equals("download")) {
                    tos.writeUTF(input);
                    tos.flush();
    
                    System.out.println("Enter filename to save: ");
                    String fileName = sc.nextLine();
                    
                    Thread t = new Thread(new FileDownloader(textSocket, fileSocket, fileName));
                    t.start();
                }
                else {
                    tos.writeUTF(input);
                    tos.flush();
                }
            }
            catch (Exception e) {
                System.err.println("Error in sending command");
                e.printStackTrace(System.err);
            }
        }
    }
}
