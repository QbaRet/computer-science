-- naglowek
with Ada.Text_IO; use Ada.Text_IO;
procedure Main is
   S : String := "to -- nie komentarz ""a -- tez nie""";  -- komentarz
   C : Character := '"';   -- cudzyslow jako znak
   D : Character := '-';
begin
   Put_Line (S); -- wypisz
end Main;
