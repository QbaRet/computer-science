package go;

import go.client.GoClient;
import go.logic.Protocol;
import go.server.GoServer;

import java.io.IOException;
import java.net.ServerSocket;

public class Main {
    public static void main(String[] args) {
        if (isPortAvailable()) {
            System.out.println("Nie wykryto serwera. Uruchamianie w trybie SERWERA");
            GoServer.main(args);
        } else {
            System.out.println("Serwer już działa. Uruchamianie w trybie KLIENTA");
            GoClient.main(args);
        }
    }

    private static boolean isPortAvailable() {
        try (ServerSocket serverSocket = new ServerSocket(Protocol.Port)) {
            return true;
        } catch (IOException e) {
            return false;
        }
    }
}
