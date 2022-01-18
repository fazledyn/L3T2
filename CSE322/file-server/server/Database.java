package server;

import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

public class Database {
    
    private static final Database instance = new Database();
    public final Queue<String> fileQueue;
    private final ArrayList<StudentModel> studentList;
    private final ArrayList<FileModel> fileList;
    private final ArrayList<RequestModel> requestList;
    private int bufferSize;
    
    private Database() {
        this.fileQueue = new LinkedList<>();
        this.studentList = new ArrayList<>();
        this.fileList = new ArrayList<>();
        this.requestList = new ArrayList<>();
        this.bufferSize = 0;
    }
    
    public static Database get() {
        return instance;
    }
    
    int getBuffer() {
        return this.bufferSize;
    }
    
    void setBuffer(int bufferSize) {
        this.bufferSize = bufferSize;
    }
    
    boolean isBufferOverflow(int fileSize) {
        return fileSize + bufferSize > _CONFIG_.MAX_BUFFER_SIZE;
    }
    
    void addStudent(String studentId) {
        studentList.add(new StudentModel(studentId, true));
    }
    
    StudentModel getStudent(String studentId) {
        for (StudentModel each : studentList) {
            if (each.studentId.equals(studentId)) {
                return each;
            }
        }
        return null;
    }
    
    int addRequest(String studentId, String description) {
        requestList.add(new RequestModel(studentId, description));
        return requestList.size() - 1;
    }
    
    void addMessage(String studentId, String message) {
        if (getStudent(studentId) != null) {
            getStudent(studentId).messages.add(message);
        }
    }
    
    void addBroadcastMessage(String senderId, String message) {
        for (StudentModel each : studentList) {
            if (!each.studentId.equals(senderId))
                each.messages.add(message);
        }
    }
    
    ArrayList<String> getMessages(String studentId) {
        if (getStudent(studentId) != null) {
            return getStudent(studentId).messages;
        }
        return new ArrayList<>();
    }
    
    void removeMessages(String studentId) {
        if (getStudent(studentId) != null) {
            getStudent(studentId).messages.clear();
        }
    }
    
    void logoutStudent(String studentId) {
        if (getStudent(studentId) != null) {
            getStudent(studentId).isOnline = false;
        }
    }
    
    boolean isStudentOnline(String studentId) {
        if (getStudent(studentId) != null) {
            return getStudent(studentId).isOnline;
        }
        return false;
    }
    
    int saveAsRequestFile(ArrayList<byte[]> fileContent, String fileName, String studentId, int requestId) {
        int fileId = saveAsFile(fileContent, fileName, "public", studentId);
        requestList.get(requestId).fileIndexList.add(fileId);
        
        String requestStudentId = requestList.get(requestId).studentId;
        String message = "Student `" + studentId + "` has added a file against your request.\n" +
                "FileID# " + fileId;
        addMessage(requestStudentId, message);
        return fileId;
    }
    
    int saveAsFile(ArrayList<byte[]> fileContent, String fileName, String fileType, String studentId) {
        try {
            String filePath = "files/" + studentId + "/" + fileType + "/" + fileName;
            FileOutputStream fos = new FileOutputStream(filePath, false);
            for (byte[] each : fileContent) {
                fos.write(each);
                fos.flush();
            }
            fos.close();
            fileList.add(new FileModel(studentId, fileName, fileType));
            return fileList.size() - 1;
        }
        catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }
    
    
    ArrayList<String> getStudentList(String studentId) {
        ArrayList<String> list = new ArrayList<>();
        for (StudentModel each : studentList) {
            if (each.studentId.equals(studentId)) list.add(each.studentId + "\t\t\t" + "[YOU]");
            else if (each.isOnline) list.add(each.studentId + "\t\t\t" + "[ONLINE]");
            else list.add(each.studentId + "\t\t\t");
        }
        return list;
    }
    
    ArrayList<String> getFilesList(String ownId, String studentId) {
        ArrayList<String> returnList = new ArrayList<>();
        
        if (!ownId.equals(studentId)) {
            // others public files
            for (int i = 0; i < fileList.size(); i++) {
                if (fileList.get(i).fileType.equals("public")) {
                    if (fileList.get(i).studentId.equals(studentId)) {
                        returnList.add(i + "\t\t" + fileList.get(i).fileName + "\t\t[" + fileList.get(i).fileType + "]");
                    }
                }
            }
            return returnList;
        }
        
        for (int i = 0; i < fileList.size(); i++) {
            if (fileList.get(i).studentId.equals(ownId)) {
                returnList.add(i + "\t\t" + fileList.get(i).fileName + "\t\t[" + fileList.get(i).fileType + "]");
            }
        }
        return returnList;
    }
    
    
    File getFile(int fileId, String ownId) {
        String fileName = fileList.get(fileId).fileName;
        String fileType = fileList.get(fileId).fileType;
        String ownerId = fileList.get(fileId).studentId;
        
        if (fileType.equals("private") && (!ownerId.equals(ownId))) {
            return null;
        }
        return new File("files/" + ownerId + "/" + fileType + "/" + fileName);
    }
    
    private static class FileModel {
        public String studentId;
        public String fileType;
        public String fileName;
        
        FileModel(String studentId, String fileName, String fileType) {
            this.fileName = fileName;
            this.studentId = studentId;
            this.fileType = fileType;
        }
    }
    
    private static class StudentModel {
        public String studentId;
        public boolean isOnline;
        public ArrayList<String> messages;
        
        StudentModel(String studentId, boolean isOnline) {
            this.studentId = studentId;
            this.isOnline = isOnline;
            this.messages = new ArrayList<>();
        }
    }
    
    private static class RequestModel {
        public String studentId;
        public String description;
        public ArrayList<Integer> fileIndexList;
        
        RequestModel(String studentId, String description) {
            this.studentId = studentId;
            this.description = description;
            this.fileIndexList = new ArrayList<>();
        }
    }
}

