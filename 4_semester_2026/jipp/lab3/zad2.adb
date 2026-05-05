with Ada.Text_IO; use Ada.Text_IO;
with Ada.Exceptions;

procedure zad2_rsa is
   
   package LLI_IO is new Ada.Text_IO.Integer_IO(Long_Long_Integer);
   use LLI_IO;

   generic
      n : Long_Long_Integer;
   package Ring_Pkg is
      type Ring is private;
      function To_Ring(X : Long_Long_Integer) return Ring;
      function "+"(L, R : Ring) return Ring;
      function "-"(L, R : Ring) return Ring;
      function "*"(L, R : Ring) return Ring;
      function "="(L, R : Ring) return Boolean;
      procedure Put(Item : Ring);
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

      function "="(L, R : Ring) return Boolean is
      begin return L.a = R.a; end "=";

      procedure Put(Item : Ring) is
      begin LLI_IO.Put(Item.a, Width => 0); end Put;
   end Ring_Pkg;


   function GCD(A, B : Long_Long_Integer) return Long_Long_Integer is
      Temp, X, Y : Long_Long_Integer;
   begin
      X := A; Y := B;
      while Y /= 0 loop
         Temp := Y;
         Y := X mod Y;
         X := Temp;
      end loop;
      return X;
   end GCD;



   generic
      type Element is private;
      with function To_Element(X : Long_Long_Integer) return Element;
      with function "*"(L, R : Element) return Element;
   package RSA_Pkg is
      type RSA_Ctx is private;
      function Create(P, Q : Long_Long_Integer) return RSA_Ctx;
      function Get_Modulo(Ctx : RSA_Ctx) return Long_Long_Integer;
      function Get_Public_Key(Ctx : RSA_Ctx) return Element;
      function Encrypt(Ctx : RSA_Ctx; M : Element) return Element;
      function Decrypt(Ctx : RSA_Ctx; C : Element) return Element;
   private
      type RSA_Ctx is record
         n : Long_Long_Integer;
         e : Long_Long_Integer;
         d : Long_Long_Integer;
      end record;
   end RSA_Pkg;

   package body RSA_Pkg is
      function Create(P, Q : Long_Long_Integer) return RSA_Ctx is
         Res : RSA_Ctx;
         Fi : Long_Long_Integer;
         T, New_T, R, New_R, Quotient, Temp_T, Temp_R : Long_Long_Integer;
      begin
         Res.n := P * Q;
         Fi := (P - 1) * (Q - 1);

         Res.e := 3;
         while Res.e < Fi loop
            if GCD(Res.e, Fi) = 1 then
               exit;
            end if;
            Res.e := Res.e + 1;
         end loop;

         T := 0; New_T := 1;
         R := Fi; New_R := Res.e;
         while New_R /= 0 loop
            Quotient := R / New_R;
            
            Temp_T := T - Quotient * New_T;
            T := New_T;
            New_T := Temp_T;
            
            Temp_R := R - Quotient * New_R;
            R := New_R;
            New_R := Temp_R;
         end loop;

         if T < 0 then
            T := T + Fi;
         end if;
         Res.d := T;

         return Res;
      end Create;

      function Get_Modulo(Ctx : RSA_Ctx) return Long_Long_Integer is
      begin return Ctx.n; end Get_Modulo;

      function Get_Public_Key(Ctx : RSA_Ctx) return Element is
      begin return To_Element(Ctx.e); end Get_Public_Key;


      function Mod_Exp(Base : Element; Exp : Long_Long_Integer) return Element is
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
      end Mod_Exp;

      function Encrypt(Ctx : RSA_Ctx; M : Element) return Element is
      begin return Mod_Exp(M, Ctx.e); end Encrypt;

      function Decrypt(Ctx : RSA_Ctx; C : Element) return Element is
      begin return Mod_Exp(C, Ctx.d); end Decrypt;
   end RSA_Pkg;


   P : constant Long_Long_Integer := 10007;
   Q : constant Long_Long_Integer := 10009;
   N : constant Long_Long_Integer := P * Q;

   package My_Ring is new Ring_Pkg(N);
   use My_Ring;

   package My_RSA is new RSA_Pkg(Element    => Ring,
                                 To_Element => To_Ring,
                                 "*"        => "*");
   use My_RSA;

   RSA_Engine : RSA_Ctx;
   Wiadomosc  : Ring;
   Szyfrogram : Ring;
   Zdekodowana : Ring;

begin
   begin
      RSA_Engine := Create(P, Q);

      Put_Line("--- Parametry RSA ---");
      Put("Modulo (n): "); LLI_IO.Put(Get_Modulo(RSA_Engine), Width => 0); New_Line;
      Put("Klucz publiczny (e): "); My_Ring.Put(Get_Public_Key(RSA_Engine)); New_Line;
      New_Line;

      Wiadomosc := To_Ring(1234567);
      Put_Line("--- Szyfrowanie ---");
      Put("Oryginalna wiadomosc: "); My_Ring.Put(Wiadomosc); New_Line;

      Szyfrogram := Encrypt(RSA_Engine, Wiadomosc);
      Put("Szyfrogram: "); My_Ring.Put(Szyfrogram); New_Line;

      Zdekodowana := Decrypt(RSA_Engine, Szyfrogram);
      Put("Zdekodowana wiadomosc: "); My_Ring.Put(Zdekodowana); New_Line;
      New_Line;

      if Wiadomosc = Zdekodowana then
         Put_Line("Sukces! Wiadomosci sie zgadzaja.");
      else
         Put_Line("Blad! Wiadomosci sa rozne.");
      end if;

   exception
      when E : others =>
         Put_Line("Wyjatek: " & Ada.Exceptions.Exception_Message(E));
   end;
end zad2_rsa;