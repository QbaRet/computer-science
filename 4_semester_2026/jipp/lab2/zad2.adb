with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO;
with Ada.Exceptions;
with Ada.Characters.Handling; use Ada.Characters.Handling;

procedure zad2 is
   generic
      n : Positive;
   package Ring_Pkg is
      type Ring is private;
      function To_Ring(X : Natural) return Ring;
      function "+"(L, R : Ring) return Ring;
      function "-"(L, R : Ring) return Ring;
      function "*"(L, R : Ring) return Ring;
      function "/"(L, R : Ring) return Ring;
      function "="(L, R : Ring) return Boolean;
      function "<"(L, R : Ring) return Boolean;
      function "<="(L, R : Ring) return Boolean;
      function ">"(L, R : Ring) return Boolean;
      function ">="(L, R : Ring) return Boolean;
      procedure Put(Item : Ring);
      procedure Get(Item : out Ring);
      Brak_Rozwiazan : exception;
   private
      type Ring is record
         a : Natural := 0;
      end record;
   end Ring_Pkg;

   package body Ring_Pkg is
      function To_Ring(X : Natural) return Ring is
      begin return (a => X mod n); end To_Ring;
      function "+"(L, R : Ring) return Ring is
      begin return (a => (L.a + R.a) mod n); end "+";
      function "-"(L, R : Ring) return Ring is
      begin return (a => (n + L.a - R.a) mod n); end "-";
      function "*"(L, R : Ring) return Ring is
      begin return (a => (L.a * R.a) mod n); end "*";
      function "/"(L, R : Ring) return Ring is
      begin
         for i in 1 .. n - 1 loop
            if (R.a * i) mod n = 1 then
               return (a => (L.a * i) mod n);
            end if;
         end loop;
         raise Brak_Rozwiazan with "Brak rozwiazan";
      end "/";
      function "="(L, R : Ring) return Boolean is
      begin return L.a = R.a; end "=";
      function "<"(L, R : Ring) return Boolean is
      begin return L.a < R.a; end "<";
      function "<="(L, R : Ring) return Boolean is
      begin return L.a <= R.a; end "<=";
      function ">"(L, R : Ring) return Boolean is
      begin return L.a > R.a; end ">";
      function ">="(L, R : Ring) return Boolean is
      begin return L.a >= R.a; end ">=";
      procedure Put(Item : Ring) is
      begin Ada.Integer_Text_IO.Put(Item.a, Width => 0); end Put;
      procedure Get(Item : out Ring) is
         Temp : Natural;
      begin
         Ada.Integer_Text_IO.Get(Temp);
         Item := To_Ring(Temp);
      end Get;
   end Ring_Pkg;

   package Ring_7 is new Ring_Pkg (n => 7);
   use Ring_7;

   a : Ring := To_Ring(10);
   b : Ring := To_Ring(5);
   c : Ring;
   zero : Ring := To_Ring(0);
begin
   begin
      Put("a: "); Put(a); New_Line;
      Put("b: "); Put(b); New_Line;
      Put("a + b = "); Put(a + b); New_Line;
      Put("a - b = "); Put(a - b); New_Line;
      Put("a * b = "); Put(a * b); New_Line;
      Put("a / b = "); Put(a / b); New_Line;

      Put_Line("a == b: " & To_Lower(Boolean'Image(a = b)));
      Put_Line("a != b: " & To_Lower(Boolean'Image(not (a = b))));
      Put_Line("a < b:  " & To_Lower(Boolean'Image(a < b)));
      Put_Line("a > b:  " & To_Lower(Boolean'Image(a > b)));
      Put_Line("a <= 3: " & To_Lower(Boolean'Image(a <= To_Ring(3))));

      c := a;
      Put("c = a:     "); Put(c); New_Line;
      c := c + b;
      Put("c += b:    "); Put(c); New_Line;
      c := c - b;
      Put("c -= b:    "); Put(c); New_Line;
      c := c * b;
      Put("c *= b:    "); Put(c); New_Line;
      c := c / b;
      Put("c /= b:    "); Put(c); New_Line;

      Put_Line("Podaj liczbe: ");
      Get(c);
      Put("Wprowadzono "); Put(c); New_Line;

      Put("Dzielenie przez 0 ");
      Put(a / zero); New_Line;

   exception
      when E : others =>
         Put_Line("wyjatek:" & Ada.Exceptions.Exception_Message(E));
   end;
end zad2;