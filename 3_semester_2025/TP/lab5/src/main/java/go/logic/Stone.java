    package go.logic;

//Klasa typu enum określająca mozliwe wartości pola na planszy
    public enum Stone {
        BLACK,
        WHITE,
        EMPTY;


        //metoda pomocnicza która zwraca kolor kamienia przeciwnika
        public Stone opponent() {
            if (this == BLACK) return WHITE;
            if (this == WHITE) return BLACK;
            return EMPTY;
        }
    }
