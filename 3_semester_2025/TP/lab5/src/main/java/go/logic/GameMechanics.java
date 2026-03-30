package go.logic;

//Klasa przechowująca zasady rozgrywki w grę Go

import java.awt.Point;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Set;

public class GameMechanics {
    public int blackCaptures = 0;
    public int whiteCaptures = 0;

    private Board recentMoveBoardCopy = new Board(19); // zmienna pomocnicza przechowujaca stan planszy z ostatniego ruchu
                                                             // - w celu okreslenia czy wystepuje zasada ko
    private Board boardAfterFutureMove = new Board(19);

    //Metoda sprawdzająca, czy mozliwe jest mozliwy do wykonania
    public boolean IsMovePossible(Board board, int x, int y, Stone color) {
        if (!board.isFieldOnBoard(x, y)) return false; // jezeli wskazane pole nie nalezy do planszy - zwracamy false
        if (board.getField(x, y) != Stone.EMPTY ) return false; //jezeli aktualnie lezy jakis kamien na wskazanym polu - zwracamy false
        board.copyBoard(boardAfterFutureMove); // ze wzgledu na zlozona logike bicia grup, analizujemy legalnosc ruchu na kopii aktualnego stanu planszy
        boardAfterFutureMove.setField(x, y, color); // Sprawdzamy, czy po wstawieniu kamienia występuje bicie lub czy wstawiony kamien ma jeszcze
        //wolne oddechy

        CheckCaptures(boardAfterFutureMove, x, y, color); // sprawdzamy, czy nie wystapilo bicie

        if (!boardAfterFutureMove.equals(recentMoveBoardCopy) && !checkSuicide(boardAfterFutureMove, x, y, color)) {// jezeli jest co najmniej jedno miejsce sąsiednie niezajęte
            // oraz przypadek ko nie wystepuje - ruch jest mozliwy
            board.copyBoard(recentMoveBoardCopy); // kopiujemy zawartosc kopii roboczej do zmiennej przechowujacej stan planszy po ostatnim ruchu
            boardAfterFutureMove.copyBoard(board); // oraz do glownej zmiennej
            return true;
        }
        return false;
    }

//    private boolean isFieldSurroundedByOpponent(Board board, int x, int y, Stone color) {
//
//        for (Direction d : Direction.values()) { // przy uzyciu wartosci enum Direction sprawdzamy, czy jakiekolwiek
//            // sasiednie pole jest wolne lub czy jest zajete przez pionek tego samego koloru
//            int newX = x + d.getDx();
//            int newY = y + d.getDy();
//            Point placedStone = new Point(x, y);
//
//            if (board.isFieldOnBoard(newX, newY)) {
//                if (board.getField(newX, newY) == Stone.EMPTY) {
//                    return false;
//                }
//                else if (board.getField(newX, newY) == color) {
//                    Point point = new Point(newX, newY);
//                    Set<Point> groupMembers = new HashSet<>();
//                    Set<Point> emptyGroupFields = new HashSet<>();
//                    Set<Point> processedTeammates = new HashSet<>();
//
//                    if (!processedTeammates.contains(point)) {
//                        groupMembers.clear();
//                        emptyGroupFields.clear();
//                        exploreGroup(board, placedStone, color, groupMembers, emptyGroupFields);
//                        if (emptyGroupFields.isEmpty()) {
//                            for (Point p : groupMembers) {
//                                board.setField(p.x, p.y, Stone.EMPTY);
//                            }
//                            groupMembers.clear();
//                        }
//                        processedTeammates.addAll(groupMembers);
//                    }
//                }
//            }
//        }
//        return true;
//    }

    private void CheckCaptures(Board board, int x, int y, Stone color) {
        Set<Point> groupMembers = new HashSet<>(); //zbior przechowujacy pionki analizowanej grupy,
                                                    // do ktorej nalezy sasiad postawionego pionka w metodzie IsMovePossible
        Set<Point> emptyGroupFields = new HashSet<>(); // zbior przechowujacy wszystkie wolne pola analizowanej grupy groupMembers
                                                        // (potrzebna do sprawdzenia, czy zbior ten nie jest pusty)
        Set<Point> processedOpponents = new HashSet<>(); // zbior przechowujacy wszystkich oponentow odwiedzonych przez algorytm
                                                    // (aby poprawnie zbadac wszystkie grupy, do ktorych naleza pionki przeciwnika sasiadujace
                                                        // z postawionym pionkiem)

        for (Direction d : Direction.values()) { // sprawdzamy kazde sasiednie pole
            int newX = x + d.getDx();
            int newY = y + d.getDy();

            if (board.isFieldOnBoard(newX, newY) && board.getField(newX, newY) == color.opponent()) {
                Point opponentPoint = new Point(newX, newY); // tworzymy nowy punkt, ktory przedstawia wspolrzedne pionka przeciwnika - sasiada pionka postawionego
                if (!processedOpponents.contains(opponentPoint)) {
                    //jezeli dany pionek przeciwnika nie nalezy do zbioru pionkow przetworzonych, uruchamiany jest algorytm exploreGroup dla tego pionka
                    groupMembers.clear();
                    emptyGroupFields.clear();
                    exploreGroup(board, opponentPoint, color.opponent(), groupMembers, emptyGroupFields);
                    if (emptyGroupFields.isEmpty()) {
                        //jezeli grupa nie posiada oddechow, zostaje ona zbita, a graczowi dopisujemy kolejnych jencow
                        //todo:policz jencow
                        for (Point point : groupMembers) {
                            board.setField(point.x, point.y, Stone.EMPTY);
                        }
                        groupMembers.clear();
                    }
                    processedOpponents.addAll(groupMembers); // dodajemy wszystkie niezbite pionki do zbioru pionkow przetworzonych
                    // (jezeli usuniete zostana zdjete z planszy, to i tak nie zostana przetworzone pozniej przez algorytm)
                }
            }
        }
    }

    public void exploreGroup(Board board, Point startPoint, Stone color, Set<Point> groupMembers, Set<Point> emptyGroupFields) {
        //algorytm bfs przeszukujacy wszystkicb sasiadow wskazanego opponenta
        //w tym algorytmie hashset groupMembers pelni role kontrolera visited danych pol na planszy
        Queue<Point> queue = new LinkedList<>();

        queue.add(startPoint);
        groupMembers.add(startPoint);


        while (!queue.isEmpty()) {
            Point current = queue.poll(); // wyjmujemy pierwszy element z kolejki
            groupMembers.add(current); // dodajemy go do zbioru grupy

            for (Direction d : Direction.values()) { // sprawdzamy kazde sasiednie pole
                int newX = current.x + d.getDx();
                int newY = current.y + d.getDy();

                if (board.isFieldOnBoard(newX, newY)) {
                    Point newPoint = new Point(newX, newY); // punkt rozwazanego sasiada pionka startPoint

                    if (board.getField(newX, newY) == color && !(groupMembers.contains(newPoint))) {
                        // dodajemy do grupy i kolejki jezeli  sasiad jest tego samego koloru oraz nie nalezy jeszcze do grupy
                        groupMembers.add(newPoint);
                        queue.add(newPoint);
                    }
                    else if (board.getField(newX, newY) == Stone.EMPTY) {
                        emptyGroupFields.add(newPoint);
                    }

                }
            }
        }
    }

    private boolean checkSuicide(Board board, int x, int y, Stone color) {
        Set<Point> myGroup = new HashSet<>();
        Set<Point> myLiberties = new HashSet<>();

        // Badamy grupę kamienia, którego właśnie postawiliśmy
        exploreGroup(board, new Point(x, y), color, myGroup, myLiberties);

        // Jeśli zbiór oddechów jest pusty, to jest samobójstwo, czyli zwracamy true
        return myLiberties.isEmpty();
    }
}
