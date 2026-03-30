package go.client;


//klasa umozliwiajaca klientowi uzywanie koordynatow stosujac zapis [litera][liczba] i tlumaczy
// ten zapis na [liczba][liczba] - wzorzec ADAPTER
public class TranslateCoordinate {
    public static int[] translate(String input) {
        if (input.length() < 2 || input.length() > 3) return null;

        input = input.toUpperCase();
        char letter = input.charAt(0);

        int x = letter - 'A';
        if (x < 0 || x >= 19) return null;

        try {
            int y = Integer.parseInt(input.substring(1)) - 1;

            if (y >= 0 && y < 19) return new int[]{x, y};
        } catch (NumberFormatException e) {
            return null;
        }

        return null;
    }

    public static char invertTranslate(int number) {
        return (char) (number + 'A');
    }
}