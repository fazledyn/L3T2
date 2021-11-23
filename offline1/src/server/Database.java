package server;

import java.io.*;
import java.net.*;
import java.util.*;

public class Database {

    private static final Database instance = new Database();
    public static Database getInstance() { return instance; }

    private final ArrayList<StudentMap> studentList;
    private final ArrayList<FileMap> privateFileList;
    private final ArrayList<FileMap> publicFileList;
    private final ArrayList<RequestMap> requestList;

    private Database() {
        studentList = new ArrayList<>();
        privateFileList = new ArrayList<>();
        publicFileList = new ArrayList<>();
        requestList = new ArrayList<>();
    }

    void addStudent(String studentId, Socket socket) {
        studentList.add(new StudentMap(studentId, true, socket));
    }

    StudentMap getStudent(String studentId) {
        for (StudentMap each : studentList) {
            if (each.studentId.equals(studentId)) {
                return each;
            }
        }
        return null;
    }

    int addRequest(String studentId, String description) {
        requestList.add(new RequestMap(studentId, description));
        return requestList.size() - 1;
    }

    void addMessage(String studentId, String message) {
        if (getStudent(studentId) != null) {
            getStudent(studentId).messages.add(message);
        }
    }

    void addBroadcastMessage(String senderId, String message) {
        for (StudentMap each : studentList) {
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

    void saveAsPublicFile(byte[] fileContent, String fileName, String studentId) throws IOException {
        FileOutputStream fos = new FileOutputStream("/files/" + studentId + "/public/" + fileName);
        fos.write(fileContent);
        publicFileList.add(new FileMap(studentId, fileName));
    }

    void saveAsPrivateFile(byte[] fileContent, String fileName, String studentId) throws IOException {
        FileOutputStream fos = new FileOutputStream("/files/" + studentId + "/private/" + fileName);
        fos.write(fileContent);
        privateFileList.add(new FileMap(studentId, fileName));
    }

    ArrayList<String> getStudentList(String studentId) {
        ArrayList<String> list = new ArrayList<>();
        for (StudentMap each : studentList) {
            if(each.studentId.equals(studentId)) list.add(each.studentId + "\t\t\t" + "[YOU]");
            else if (each.isOnline)              list.add(each.studentId + "\t\t\t" + "[ONLINE]");
            else                                 list.add(each.studentId + "\t\t\t");
        }
        return list;
    }

    ArrayList<String> getFilesList(String ownId, String studentId) {
        ArrayList<String> fileList = new ArrayList<>();
        for (int i=0; i < publicFileList.size(); i++) {
            if (publicFileList.get(i).studentId.equals(studentId)) {
                fileList.add(i + "\t\t" + publicFileList.get(i).fileName + "\t\t [PUBLIC]");
            }
        }
        for (int i=0; i < privateFileList.size(); i++) {
            if (privateFileList.get(i).studentId.equals(ownId)) {
                fileList.add(i + "\t\t" + privateFileList.get(i).fileName + "\t\t [PRIVATE]");
            }
        }
        return fileList;
    }

    File getPublicFile(int fileId) {
        String fileName = publicFileList.get(fileId).fileName;
        String studentId = publicFileList.get(fileId).studentId;
        try {
            return new File("/files/" + studentId + "/public/" + fileName);
        }
        catch (Exception e) {
            return null;
        }
    }

    File getPrivateFile(int fileId, String ownId) {
        String fileName = privateFileList.get(fileId).fileName;
        String fileOwnerId = privateFileList.get(fileId).studentId;
        try {
            if (!fileOwnerId.equals(ownId)) return null;
            else return new File("/files/" + ownId + "/private/" + fileName);
        }
        catch (Exception e) {
            return null;
        }
    }
}

class FileMap {
    public String studentId;
    public String fileName;

    FileMap(String studentId, String fileName) {
        this.fileName = fileName;
        this.studentId = studentId;
    }
}

class StudentMap {
    public String studentId;
    public boolean isOnline;
    public Socket socket;
    public ArrayList<String> messages;

    StudentMap(String studentId, boolean isOnline, Socket socket) {
        this.studentId = studentId;
        this.isOnline = isOnline;
        this.socket = socket;
        this.messages = new ArrayList<>();
    }
}

class RequestMap {
    public String studentId;
    public String description;
    public ArrayList<Integer> fileIndexList;

    RequestMap(String studentId, String description) {
        this.studentId = studentId;
        this.description = description;
        this.fileIndexList = new ArrayList<>();
    }
}