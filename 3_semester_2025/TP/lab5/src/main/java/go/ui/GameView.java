package go.ui;

import go.logic.Board;

// Deklaracja interfejsu w celu pozniejszego latwiejszego przejscia z UI konsolowego na GUI

public interface GameView {
    void showBoard(Board board);
    void showMessage(String message); // otrzymana wiadomosc od serwera
    String getInput(); // input od gracza
}
