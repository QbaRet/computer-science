package go.server;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;

import go.logic.Board;
import go.logic.GameMechanics;
import go.logic.Protocol;
import go.logic.Stone;

public class GameSession implements Runnable{
    //gniazda sieciowe dla graczy
    private Socket p1Socket;
    private Socket p2Socket;
    private Board board;
    private GameMechanics mechanics;
    
    public GameSession(Socket p1, Socket p2){
        this.p1Socket = p1;
        this.p2Socket = p2;
        this.board = new Board(19);
        this.mechanics = new GameMechanics();
    }
    @Override
    public void run() {
        try{
            //strumienie wejscia i wyjscia dla obu graczy
            DataInputStream input1 = new DataInputStream(p1Socket.getInputStream());
            DataOutputStream output1 = new DataOutputStream(p1Socket.getOutputStream());

            DataInputStream input2 = new DataInputStream(p2Socket.getInputStream());
            DataOutputStream output2 = new DataOutputStream(p2Socket.getOutputStream());
            //ustawienie tablic strumieni i kolorów, aby ułatwić zarządzanie w pętli gry
            DataInputStream[] inputs = {input1, input2};
            DataOutputStream[] outputs = {output1, output2};
            Stone[] colors = {Stone.BLACK, Stone.WHITE};

            output1.writeInt(1);
            output1.flush();
            System.out.println("Gra rozpoczęta.");
            //gracze wewnatrz programu sa numerowani 0 i 1 ze wzgledu na tablice
            int currentPlayer = 0; 
            while (true) {
                int opponent = 1 - currentPlayer; //numer przeciwnika
                int messageType = inputs[currentPlayer].readInt();
            
                if (messageType == Protocol.MOVE) {
                    //odcyztanie współrzędnych ruchu
                    int x = inputs[currentPlayer].readInt();
                    int y = inputs[currentPlayer].readInt();
                    //sprwadzenie czy ruch jest legalny
                    if (mechanics.IsMovePossible(board, x, y, colors[currentPlayer])) {
                        System.out.println("Gracz " + (currentPlayer + 1) + " wykonał ruch na pozycję (" + x + "," + y + ")");
                        //Przekazanie stanu planszy i liczby jeńców obu graczom
                        outputs[currentPlayer].writeInt(Protocol.BOARD_STATE);
                        Protocol.sendBoard(board, outputs[currentPlayer]);

                        outputs[currentPlayer].writeInt(Protocol.CAPTURES);
                        outputs[currentPlayer].writeInt(mechanics.blackCaptures);
                        outputs[currentPlayer].writeInt(mechanics.whiteCaptures);
                        outputs[currentPlayer].flush();
                        //wysłanie informacji o ruchu przeciwnikowi, tak aby UI mogło zaktualizować planszę
                        outputs[opponent].writeInt(Protocol.BOARD_STATE);
                        Protocol.sendBoard(board, outputs[opponent]);

                        outputs[opponent].writeInt(Protocol.CAPTURES);
                        outputs[opponent].writeInt(mechanics.blackCaptures);
                        outputs[opponent].writeInt(mechanics.whiteCaptures);

                        // Informacja o ruchu (żeby UI przeciwnika wiedziało, że teraz jego kolej)
                        outputs[opponent].writeInt(Protocol.MOVE);
                        outputs[opponent].writeInt(x);
                        outputs[opponent].writeInt(y);
                        
                        outputs[opponent].flush();
                        //zmiana tury
                        currentPlayer = opponent;
                    } else {
                        //gdy ruch był nielegalny informujemy gracza i nie zmienyamy tury
                        System.out.println("Gracz " + (currentPlayer + 1) + " wykonał nielegalny ruch na pozycję (" + x + "," + y + ")");
                        outputs[currentPlayer].writeInt(Protocol.INVALID_MOVE);
                        outputs[currentPlayer].writeInt(x);
                        outputs[currentPlayer].writeInt(y);
                        outputs[currentPlayer].flush();
                    }
                } else if (messageType == Protocol.PASS) {
                    //w przypadku passu przekazujemy informację przeciwnikowi
                    System.out.println("Gracz " + (currentPlayer + 1) + " pasuje.");
                    outputs[opponent].writeInt(Protocol.PASS);
                    outputs[opponent].flush();
                    currentPlayer = opponent;
                } else if (messageType == Protocol.SURRENDER) {
                    //gdy gracz sie podda przekazujemy informację przeciwnikowi i kończymy grę
                    System.out.println("Gracz " + (currentPlayer + 1) + " się poddał. Gracz " + (opponent + 1) + " wygrywa.");
                    outputs[opponent].writeInt(Protocol.SURRENDER);
                    outputs[opponent].flush();
                    break;
                } else if (messageType == Protocol.QUIT) {
                    //gdy gracz wychodzi z gry przekazujemy informację przeciwnikowi i kończymy grę
                    System.out.println("Gracz " + (currentPlayer + 1) + " wyszedł z gry.");
                    outputs[opponent].writeInt(Protocol.QUIT);
                    outputs[opponent].flush();
                    break;
                }
            }

        } catch (Exception e) {
            System.out.println("Któryś z graczy rozłączył się.");
        }
    }
}