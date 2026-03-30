package go.logic;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class GameMechanicsTest {

    @Test
    void acceptsValidMoveOnEmptyField() {
        Board board = new Board(5);
        GameMechanics mechanics = new GameMechanics();

        boolean result = mechanics.IsMovePossible(board, 1, 1, Stone.BLACK);

        assertTrue(result);
        assertEquals(Stone.BLACK, board.getField(1, 1));
    }

    @Test
    void rejectsMoveOnOccupiedField() {
        Board board = new Board(5);
        GameMechanics mechanics = new GameMechanics();
        board.setField(2, 2, Stone.WHITE);

        boolean result = mechanics.IsMovePossible(board, 2, 2, Stone.BLACK);

        assertFalse(result);
        assertEquals(Stone.WHITE, board.getField(2, 2));
    }

    @Test
    void capturesAdjacentStoneWithoutLiberties() {
        Board board = new Board(3);
        GameMechanics mechanics = new GameMechanics();

        board.setField(1, 1, Stone.WHITE);
        board.setField(0, 1, Stone.BLACK);
        board.setField(1, 0, Stone.BLACK);
        board.setField(2, 1, Stone.BLACK);

        boolean result = mechanics.IsMovePossible(board, 1, 2, Stone.BLACK);

        assertTrue(result);
        assertEquals(Stone.BLACK, board.getField(1, 2));
        assertEquals(Stone.EMPTY, board.getField(1, 1));
        assertEquals(1, mechanics.blackCaptures);
    }
}
