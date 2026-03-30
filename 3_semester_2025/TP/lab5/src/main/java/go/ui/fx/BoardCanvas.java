package go.ui.fx;

import go.logic.Board;
import go.logic.Stone;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;

public class BoardCanvas extends Canvas {
    private final double cellSize = 35.0; //odstep miedzy liniami
    private final double offset = 40.0;   //marginesy

    public void draw(Board board) {
        GraphicsContext gc = getGraphicsContext2D();
        int size = board.getSize(); 

        gc.setFill(Color.web("#DEB887")); //ustawienie koloru tla
        gc.fillRect(0, 0, getWidth(), getHeight());
        //rysowanie siatki
        gc.setStroke(Color.BLACK);
        gc.setLineWidth(1.0);
        for (int i = 0; i < size; i++) {
            // linie poziome
            gc.strokeLine(offset, offset + i * cellSize, offset + (size - 1) * cellSize, offset + i * cellSize);
            // linie pionowe
            gc.strokeLine(offset + i * cellSize, offset, offset + i * cellSize, offset + (size - 1) * cellSize);
        }

        // rysowanie kamieni na podstawie stanu planszy
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                Stone stone = board.getField(x, y); //pobranie stanu pola
                if (stone != Stone.EMPTY) {
                    renderStone(gc, x, y, stone);
                }
            }
        }
    }

    private void renderStone(GraphicsContext gc, int x, int y, Stone color) {
        double r = cellSize * 0.45; // promień kamienia
        double centerX = offset + x * cellSize;
        double centerY = offset + y * cellSize;

        gc.setFill(Color.rgb(0, 0, 0, 0.3));
        gc.fillOval(centerX - r + 2, centerY - r + 2, r * 2, r * 2);

        // kamień właściwy
        gc.setFill(color == Stone.BLACK ? Color.BLACK : Color.WHITE);
        gc.fillOval(centerX - r, centerY - r, r * 2, r * 2);

        // obwódka
        gc.setStroke(Color.BLACK);
        gc.setLineWidth(0.5);
        gc.strokeOval(centerX - r, centerY - r, r * 2, r * 2);
    }

    // metoda pomocnicza dla GUIView do zamiany kliknięcia na indeks tablicy
    public int toBoardCoord(double pixel) {
        return (int) Math.round((pixel - offset) / cellSize);
    }
}