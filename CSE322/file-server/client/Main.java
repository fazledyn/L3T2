package client;

public class Main {
    
    public static void main(String[] args) {
        final int TEXT_PORT = 6666;
        final int FILE_PORT = 6667;
        Client client = new Client(TEXT_PORT, FILE_PORT);
        client.start();
    }
}
