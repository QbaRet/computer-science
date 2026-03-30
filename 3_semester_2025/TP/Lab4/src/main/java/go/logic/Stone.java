    package go.logic;
    public enum Stone {
        BLACK,
        WHITE,
        EMPTY;

        public Stone opponent() {
            if (this == BLACK) return WHITE;
            if (this == WHITE) return BLACK;
            return EMPTY;
        }
    }
