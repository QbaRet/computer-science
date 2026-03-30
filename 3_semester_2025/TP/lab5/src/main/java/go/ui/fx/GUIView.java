package go.ui.fx;

import java.util.concurrent.LinkedBlockingQueue;

import go.logic.Board;
import go.ui.GameView;
import javafx.application.Platform;
public class GUIView implements GameView{
    private final BoardCanvas canvas;
    private final SidePanel sidePanel;
    private final LinkedBlockingQueue<String> inputQueue = new LinkedBlockingQueue<>(); // kolejka do przechowywania wejscia od uzytkownika
    private volatile boolean waitingForInput = false;
    public GUIView(BoardCanvas canvas, SidePanel sidePanel)
    {   // inicjalizacja komponentow GUI
        this.canvas = canvas;
        this.sidePanel = sidePanel;
        setupEvents();
    }
    private void setupEvents(){
            // obsluga klikniec na planszy
        canvas.setOnMouseClicked( e->{
            if(!waitingForInput) return; // ignoruj klikniecia, jesli nie czekamy na wejscie
            int x=canvas.toBoardCoord(e.getX());
            int y=canvas.toBoardCoord(e.getY());
            // konwersja na notacje literowo-cyfrowa
            char letter =(char)('A'+x);
            int number = y+1;
            inputQueue.offer(""+letter+number);
        });
        // obsluga przyciskow panelu bocznego
        sidePanel.getPassBtn().setOnAction( e-> inputQueue.offer("pass"));
        sidePanel.getSurrenderBtn().setOnAction( e-> inputQueue.offer("surrender"));
    }
    //metody interfejsu GameView
    @Override
    public void showBoard(Board board){
        Platform.runLater( ()-> canvas.draw(board) );
    }
    @Override
    public void showMessage(String message){
        Platform.runLater( ()-> sidePanel.addLog(message) );
    }
    @Override
    public String getInput(){
        //wyczyszczenie klikniec z poprzednich tur
        inputQueue.clear();
        waitingForInput = true;
        // pobranie wejscia z kolejki
        try {
            String input = inputQueue.take();
            waitingForInput = false;
            return input;
        } catch (InterruptedException e) {
            return "quit";
        }
    }
}
