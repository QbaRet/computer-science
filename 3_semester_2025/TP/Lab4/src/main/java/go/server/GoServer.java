package go.server;
import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;

import go.logic.Protocol;

public class GoServer {
    public static void main(String[] args){
        new GoServer();
    }    

    public GoServer(){
        try(ServerSocket serverSocket = new ServerSocket(Protocol.Port)) {
           while(true){
                System.out.println("Oczekiwanie na graczy");
                Socket p1 = serverSocket.accept();
                System.out.println("Gracz 1 dołączył");
                new DataOutputStream(p1.getOutputStream()).writeInt(Protocol.Player1);
                Socket p2 = serverSocket.accept();
                System.out.println("Gracz 2 dołączył");
                new DataOutputStream(p2.getOutputStream()).writeInt(Protocol.Player2);
                GameSession game = new GameSession(p1, p2);
                new Thread(game).start();
           } 
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
