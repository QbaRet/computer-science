package go.client;
import java.io.IOException;

import go.logic.Board;
import go.logic.Protocol;
import go.logic.Stone;
import go.ui.ConsoleView;
import go.ui.GameView;

public class GoClient {
    private Board board = new Board(19);
    private final GameView gameView = new ConsoleView();
    private int blackCaptures = 0;
    private int whiteCaptures = 0;
    NetworkConnection network = new NetworkConnection();
    Stone myColor;
    public static void main(String[] args){
        new GoClient().connect();
    }
    private void connect(){
        try {
            gameView.showMessage("Łączenie z serwerem");
            network.connect();
            int playerId = network.getPlayerId();
            gameView.showMessage("Połączono jako gracz " + playerId);
            if(playerId == Protocol.Player1){
                gameView.showMessage("Czekanie na drugiego gracza");
                network.waitForSecondPlayer();
                myColor = Stone.BLACK;
                gameView.showMessage("Drugi gracz dołączył. Rozpoczynam grę");
            }
            else{
                myColor = Stone.WHITE;
                gameView.showMessage("Rozpoczynam grę");
            }
            playGame(playerId);
        } catch (Exception e) {
            gameView.showMessage("Błąd połączenia z serwerem");
        }
    } 
    private void playGame(int playerId) throws IOException {
        Stone currentTurn = Stone.BLACK;
        while(true){
            if(currentTurn == myColor){
                gameView.showMessage("Ilość jeńców - Gracz1: " + blackCaptures + " Gracz2: " + whiteCaptures);
                gameView.showBoard(board);

                gameView.showMessage("Twój ruch. Wpisz współrzędne '[Litera][liczba]' (np. D13) lub 'pass', 'surrender', 'quit':");
                String input = gameView.getInput();
                
                if(input.equalsIgnoreCase("pass")){
                    network.sendPassMessage();
                    gameView.showMessage("Pasujesz turę.");
                    currentTurn = currentTurn.opponent();
                } 
                else if(input.equalsIgnoreCase("surrender")){
                    network.sendSurrenderMessage();
                    gameView.showMessage("Poddajesz się. Koniec gry.");
                    break;
                }
                else if(input.equalsIgnoreCase("quit")){
                    network.sendQuitMessage();
                    gameView.showMessage("Wychodzisz z gry.");
                    break;
                }
                else {
                    try {
                        int[] coordinates = TranslateCoordinate.translate(input);
                        if (coordinates != null) {
                            network.sendSpaceInformation(coordinates);
                            gameView.showMessage("Ruch wysłany na pozycję (" + input.toUpperCase() + ")");
                            currentTurn = currentTurn.opponent();
                        } else {
                            gameView.showMessage("Nieprawidłowy format. Wpisz '[Litera][Liczba]' (np. D13) lub komendę.");
                        }
                    } catch (NumberFormatException e) {
                        gameView.showMessage("Nieprawidłowe współrzędne. Spróbuj ponownie.");
                    }
                }

            } else {
                gameView.showMessage("Czekanie na ruch przeciwnika");
            
                int messageType = network.readMessage();

                if (messageType == Protocol.MOVE) {
                    int[] coordinates = network.getCoordinates();
                    gameView.showMessage("Przeciwnik postawił kamień na: " + TranslateCoordinate.invertTranslate(coordinates[0]) + coordinates[1]);
                    currentTurn = currentTurn.opponent();
                }
                else if (messageType == Protocol.BOARD_STATE) {
                    network.receiveBoardFromServer(board);
                }
                else if (messageType == Protocol.CAPTURES) {
                    network.getCaptures();
                }
                else if (messageType == Protocol.INVALID_MOVE) {
                    int[] coordinates = network.getCoordinates();
                    gameView.showMessage("Ruch ("+TranslateCoordinate.invertTranslate(coordinates[0])+","+coordinates[1]+") jest nielegalny! Spróbuj ponownie.");
                    currentTurn = currentTurn.opponent();
                }
                else if (messageType == Protocol.PASS) {
                    gameView.showMessage("Przeciwnik spasował.");
                    currentTurn = currentTurn.opponent();
                }
                else if (messageType == Protocol.SURRENDER) {
                    gameView.showMessage("Przeciwnik się poddał! Wygrałeś.");
                    break;
                }
                else if (messageType == Protocol.QUIT) {
                    gameView.showMessage("Przeciwnik wyszedł z gry.");
                    break;
                }
            }
        }
    }
}