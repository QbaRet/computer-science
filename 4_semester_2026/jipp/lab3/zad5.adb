with Ada.Text_IO; use Ada.Text_IO;
with Ada.Exceptions;
with Ada.Numerics.Discrete_Random;

procedure zad4_dh is
   
   -- Wejscie/Wyjscie dla 64-bitowych intow
   package LLI_IO is new Ada.Text_IO.Integer_IO(Long_Long_Integer);
   use LLI_IO;

   -- ==========================================
   -- PAKIET PIERSCIENIA (Zoptymalizowany)
   -- ==========================================
   generic
      n : Long_Long_Integer;
   package Ring_Pkg is
      type Ring is private;
      function To_Ring(X : Long_Long_Integer) return Ring;
      function "+"(L, R : Ring) return Ring;
      function "-"(L, R : Ring) return Ring;
      function "*"(L, R : Ring) return Ring;
      function "/"(L, R : Ring) return Ring;
      function "="(L, R : Ring) return Boolean;
      procedure Put(Item : Ring);
      Brak_Rozwiazan : exception;
   private
      type Ring is record
         a : Long_Long_Integer := 0;
      end record;
   end Ring_Pkg;

   package body Ring_Pkg is
      function To_Ring(X : Long_Long_Integer) return Ring is
      begin return (a => X mod n); end To_Ring;

      function "+"(L, R : Ring) return Ring is
      begin return (a => (L.a + R.a) mod n); end "+";

      function "-"(L, R : Ring) return Ring is
      begin return (a => (n + L.a - R.a) mod n); end "-";

      function "*"(L, R : Ring) return Ring is
      begin return (a => (L.a * R.a) mod n); end "*";

      -- Szybkie dzielenie przy uzyciu Rozszerzonego Algorytmu Euklidesa
      function "/"(L, R : Ring) return Ring is
         T : Long_Long_Integer := 0;  New_T : Long_Long_Integer := 1;
         R_Val : Long_Long_Integer := n;  New_R : Long_Long_Integer := R.a;
         Quotient, Temp_T, Temp_R : Long_Long_Integer;
      begin
         while New_R /= 0 loop
            Quotient := R_Val / New_R;
            Temp_T := T - Quotient * New_T;
            T := New_T; New_T := Temp_T;
            Temp_R := R_Val - Quotient * New_R;
            R_Val := New_R; New_R := Temp_R;
         end loop;
         if R_Val > 1 then
            raise Brak_Rozwiazan with "Dzielenie przez zero lub brak odwrotnosci";
         end if;
         if T < 0 then
            T := T + n;
         end if;
         return (a => (L.a * T) mod n);
      end "/";

      function "="(L, R : Ring) return Boolean is
      begin return L.a = R.a; end "=";

      procedure Put(Item : Ring) is
      begin LLI_IO.Put(Item.a, Width => 0); end Put;
   end Ring_Pkg;

   -- ==========================================
   -- PROTOKOL DIFFIE-HELLMAN
   -- ==========================================
   generic
      type Element is private;
      with function To_Element(X : Long_Long_Integer) return Element;
      with function "*"(L, R : Element) return Element;
      with function "/"(L, R : Element) return Element;
   package DH_Pkg is
      
      -- Setup
      type DHSetup is tagged private;
      procedure Init(Setup : out DHSetup);
      function Get_Generator(Setup : DHSetup) return Element;
      function Power(Setup : DHSetup; Base : Element; Exp : Long_Long_Integer) return Element;

      -- User
      type User is tagged private;
      procedure Init_User(U : out User; Setup : DHSetup);
      function Get_Public_Key(U : User; Setup : DHSetup) return Element;
      procedure Set_Key(U : in out User; Setup : DHSetup; Other_Public : Element);
      function Encrypt(U : User; M : Element) return Element;
      function Decrypt(U : User; C : Element) return Element;
      
      Key_Not_Set : exception;

   private
      type DHSetup is tagged record
         Generator : Element;
      end record;
      
      type User is tagged record
         Secret : Long_Long_Integer;
         Key    : Element;
         Is_Key_Set : Boolean := False;
      end record;
   end DH_Pkg;

   package body DH_Pkg is
      
      -- Generator losowy
      subtype Rand_Range is Long_Long_Integer range 2 .. 1000000000;
      package Rand is new Ada.Numerics.Discrete_Random(Rand_Range);
      Gen : Rand.Generator;
      
      procedure Init(Setup : out DHSetup) is
      begin
         Rand.Reset(Gen);
         Setup.Generator := To_Element(Rand.Random(Gen));
      end Init;

      function Get_Generator(Setup : DHSetup) return Element is
      begin return Setup.Generator; end Get_Generator;

      function Power(Setup : DHSetup; Base : Element; Exp : Long_Long_Integer) return Element is
         Result : Element := To_Element(1);
         B : Element := Base;
         E : Long_Long_Integer := Exp;
      begin
         while E > 0 loop
            if E mod 2 = 1 then
               Result := Result * B;
            end if;
            B := B * B;
            E := E / 2;
         end loop;
         return Result;
      end Power;

      procedure Init_User(U : out User; Setup : DHSetup) is
      begin
         U.Secret := Rand.Random(Gen);
         U.Is_Key_Set := False;
      end Init_User;

      function Get_Public_Key(U : User; Setup : DHSetup) return Element is
      begin
         return Power(Setup, Get_Generator(Setup), U.Secret);
      end Get_Public_Key;

      procedure Set_Key(U : in out User; Setup : DHSetup; Other_Public : Element) is
      begin
         U.Key := Power(Setup, Other_Public, U.Secret);
         U.Is_Key_Set := True;
      end Set_Key;

      function Encrypt(U : User; M : Element) return Element is
      begin
         if not U.Is_Key_Set then raise Key_Not_Set with "Klucz nieustawiony!"; end if;
         return M * U.Key;
      end Encrypt;

      function Decrypt(U : User; C : Element) return Element is
      begin
         if not U.Is_Key_Set then raise Key_Not_Set with "Klucz nieustawiony!"; end if;
         return C / U.Key;
      end Decrypt;

   end DH_Pkg;


   -- ==========================================
   -- KONFIGURACJA TESTU
   -- ==========================================
   P : constant Long_Long_Integer := 1234567891;
   
   package My_Ring is new Ring_Pkg(P);
   use My_Ring;

   package My_DH is new DH_Pkg(Element    => Ring,
                               To_Element => To_Ring,
                               "*"        => "*",
                               "/"        => "/");
   use My_DH;

   -- Zmienne
   Setup : DHSetup;
   Alice, Bob : User;
   Pub_Alice, Pub_Bob : Ring;
   Wiadomosc, Szyfrogram, Zdekodowana : Ring;

begin
   begin
      Put_Line("--- Inicjalizacja Diffie-Hellman ---");
      Init(Setup);
      Put("Wylosowany generator ciala: "); My_Ring.Put(Get_Generator(Setup)); New_Line;
      New_Line;

      Init_User(Alice, Setup);
      Init_User(Bob, Setup);

      Pub_Alice := Get_Public_Key(Alice, Setup);
      Pub_Bob   := Get_Public_Key(Bob, Setup);

      Put("Klucz publiczny Alice: "); My_Ring.Put(Pub_Alice); New_Line;
      Put("Klucz publiczny Boba:  "); My_Ring.Put(Pub_Bob); New_Line;
      New_Line;

      -- Test wyjatku
      Put_Line("Proba szyfrowania bez klucza (Alice)...");
      begin
         Szyfrogram := Encrypt(Alice, To_Ring(123));
      exception
         when E : others => Put_Line("   Zlapano wyjatek: " & Ada.Exceptions.Exception_Message(E));
      end;
      New_Line;

      -- Wymiana kluczy
      Set_Key(Alice, Setup, Pub_Bob);
      Set_Key(Bob, Setup, Pub_Alice);

      Wiadomosc := To_Ring(987654321);
      Put_Line("--- Komunikacja (Alice -> Bob) ---");
      Put("Wiadomosc Alice: "); My_Ring.Put(Wiadomosc); New_Line;

      Szyfrogram := Encrypt(Alice, Wiadomosc);
      Put("Zaszyfrowana wiadomosc: "); My_Ring.Put(Szyfrogram); New_Line;

      Zdekodowana := Decrypt(Bob, Szyfrogram);
      Put("Zdekodowana wiadomosc: "); My_Ring.Put(Zdekodowana); New_Line;
      New_Line;

      if Wiadomosc = Zdekodowana then
         Put_Line("Sukces! Wymiana powiodla sie.");
      else
         Put_Line("Blad!");
      end if;

   exception
      when E : others => Put_Line("Wyjatek globalny: " & Ada.Exceptions.Exception_Message(E));
   end;
end zad4_dh;