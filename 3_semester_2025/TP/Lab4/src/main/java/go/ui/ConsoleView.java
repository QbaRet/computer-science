package go.ui;

import go.logic.Board;
import go.logic.Stone;
import java.util.Scanner;

public class ConsoleView implements GameView {
    private final Scanner scanner;

    public ConsoleView() {
        this.scanner = new Scanner(System.in);
    }

    @Override
    public void showBoard(Board board) {
        int size = board.getSize();

        System.out.println("A B C D E F G H I J K L M N O P Q R S");

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                if (board.getField(x, y) == Stone.EMPTY) System.out.print(". ");
                else if (board.getField(x, y) == Stone.WHITE) System.out.print("O ");
                else if (board.getField(x, y) == Stone.BLACK) System.out.print("X ");
            }
            System.out.printf("%d%n", (y + 1));
        }
    }

    @Override
    public void showMessage(String message) {
        System.out.println(message);
    }

    @Override
    public String getInput() {
        return scanner.nextLine().trim();
    }
}
