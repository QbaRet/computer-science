package go.server;
import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;

import go.logic.Protocol;
//klasa odpowiadająca za nasłuchiwanie połączeń i stworzenie sesji gry
public class GoServer {
    public static void main(String[] args){
        new GoServer();
    }    

    public GoServer(){
        //utworzenie gniazda serwera na określonym porcie
        try(ServerSocket serverSocket = new ServerSocket(Protocol.Port)) {
           while(true){
                System.out.println("Oczekiwanie na graczy");
                //oczekiwanie na pierwszego gracza
                Socket p1 = serverSocket.accept();
                System.out.println("Gracz 1 dołączył");
                new DataOutputStream(p1.getOutputStream()).writeInt(Protocol.Player1);
                //oczekiwanie na drugiego gracza
                Socket p2 = serverSocket.accept();
                System.out.println("Gracz 2 dołączył");
                new DataOutputStream(p2.getOutputStream()).writeInt(Protocol.Player2);
                //utworzenie sesji gry dla pary graczy
                GameSession game = new GameSession(p1, p2);
                new Thread(game).start();
           } 
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
