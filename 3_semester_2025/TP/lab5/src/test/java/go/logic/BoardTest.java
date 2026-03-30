package go.logic;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class BoardTest {

    @Test
    void setAndGetFieldWithinBounds() {
        Board board = new Board(5);
        board.setField(2, 3, Stone.BLACK);
        assertEquals(Stone.BLACK, board.getField(2, 3));
    }

    @Test
    void setFieldOutOfBoundsThrows() {
        Board board = new Board(3);
        assertThrows(IllegalArgumentException.class, () -> board.setField(-1, 0, Stone.WHITE));
        assertThrows(IllegalArgumentException.class, () -> board.getField(3, 3));
    }
}
