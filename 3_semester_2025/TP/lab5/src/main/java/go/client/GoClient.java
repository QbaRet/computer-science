package go.client;
import java.io.IOException;

import go.logic.Board;
import go.logic.Protocol;
import go.logic.Stone;
import go.ui.GameView;
// klasa GoClient obsługuje logikę klienta 
public class GoClient {
    private Board board = new Board(19);
    private final GameView gameView;
    private int blackCaptures = 0;
    private int whiteCaptures = 0;
    NetworkConnection network = new NetworkConnection();
    Stone myColor;
    public GoClient(GameView gameView){
        this.gameView = gameView;
    }
    public void connect(){
        try {
            gameView.showMessage("Łączenie z serwerem");
            network.connect(); // nawiązanie połączenia z serwerem
            int playerId = network.getPlayerId();
            gameView.showMessage("Połączono jako gracz " + playerId);
            if(playerId == Protocol.Player1){
                gameView.showMessage("Czekanie na drugiego gracza");
                network.waitForSecondPlayer(); // oczekiwanie na drugiego gracza
                myColor = Stone.BLACK; // przypisanie koloru czarnego pierwszemu graczowi
                gameView.showMessage("Drugi gracz dołączył. Rozpoczynam grę");
            }
            else{   
                myColor = Stone.WHITE;  // przypisanie koloru białego drugiemu graczowi
                gameView.showMessage("Rozpoczynam grę");
            }
            playGame(playerId); //start gry
        } catch (Exception e) {
            gameView.showMessage("Błąd połączenia z serwerem");
        }
    } 
    private void playGame(int playerId) throws IOException {
        Stone currentTurn = Stone.BLACK;
        while(true){
            if(currentTurn == myColor){
                // Aktualizacja stanu planszy i liczby jeńców
                gameView.showMessage("Ilość jeńców - Gracz1: " + blackCaptures + " Gracz2: " + whiteCaptures);
                gameView.showBoard(board);

                gameView.showMessage("Twój ruch. Wpisz współrzędne '[Litera][liczba]' (np. D13) lub 'pass', 'surrender', 'quit':");
                String input = gameView.getInput();
                
                if(input.equalsIgnoreCase("pass")){
                    // Wysłanie informacji o pasie do serwera
                    network.sendPassMessage();
                    gameView.showMessage("Pasujesz turę.");
                    currentTurn = currentTurn.opponent();
                } 
                else if(input.equalsIgnoreCase("surrender")){
                    // Wysłanie informacji o poddaniu się do serwera
                    network.sendSurrenderMessage();
                    gameView.showMessage("Poddajesz się. Koniec gry.");
                    break;
                }
                else if(input.equalsIgnoreCase("quit")){
                    // Wysłanie informacji o wyjściu z gry do serwera
                    network.sendQuitMessage();
                    gameView.showMessage("Wychodzisz z gry.");
                    break;
                }
                else {
                    // Proba przetlumaczenia współrzędnych i wysłania ruchu do serwera
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
                // Oczekiwanie na ruch przeciwnika
                gameView.showMessage("Czekanie na ruch przeciwnika");
            
                int messageType = network.readMessage(); // odczytanie rodzaju wiadomosci od serwera

                if (messageType == Protocol.MOVE) {
                    int[] coordinates = network.getCoordinates(); // wspolrzedne ruchu przeciwnika
                    gameView.showMessage("Przeciwnik postawił kamień na: " + TranslateCoordinate.invertTranslate(coordinates[0]) + coordinates[1]);
                    currentTurn = currentTurn.opponent();
                }
                else if (messageType == Protocol.BOARD_STATE) {
                    network.receiveBoardFromServer(board); // aktualizacja stanu planszy
                    gameView.showBoard(board);
                }
                else if (messageType == Protocol.CAPTURES) {
                    int [] captures = network.getCaptures();
                    blackCaptures = captures[0];
                    whiteCaptures = captures[1];
                }
                else if (messageType == Protocol.INVALID_MOVE) {
                    // obsługa nielegalnego ruchu przeciwnika
                    int[] coordinates = network.getCoordinates();
                    gameView.showMessage("Ruch ("+TranslateCoordinate.invertTranslate(coordinates[0])+","+coordinates[1]+") jest nielegalny! Spróbuj ponownie.");
                    currentTurn = currentTurn.opponent();
                }
                else if (messageType == Protocol.PASS) {
                    // obsługa pasa przeciwnika
                    gameView.showMessage("Przeciwnik spasował.");
                    currentTurn = currentTurn.opponent();
                }
                else if (messageType == Protocol.SURRENDER) {
                    // obsługa poddania się przeciwnika
                    gameView.showMessage("Przeciwnik się poddał! Wygrałeś.");
                    break;
                }
                else if (messageType == Protocol.QUIT) {
                    // obsługa wyjścia przeciwnika z gry
                    gameView.showMessage("Przeciwnik wyszedł z gry.");
                    break;
                }
            }
        }
    }
}