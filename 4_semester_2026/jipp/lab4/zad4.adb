with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Discrete_Random;

procedure Star_Network is
   N : constant Integer := 5; -- Liczba uzytkownikow
   M : constant Integer := 3; -- Komunikaty na uzytkownika

   -- Definicja Serwera
   task Server is
      entry Send_Message(Sender : in Integer; Target : in Integer);
   end Server;

   -- Definicja Uzytkownika
   task type User is
      entry Start(ID : Integer);
      entry Receive_Message(Sender : in Integer);
      entry Shutdown;
   end User;

   Users : array (1 .. N) of User;

   -- Cialo Serwera
   task body Server is
      Current_Sender, Current_Target : Integer;
   begin
      -- Serwer przetworzy dokladnie N * M wiadomosci
      for I in 1 .. N * M loop
         accept Send_Message(Sender : in Integer; Target : in Integer) do
            Current_Sender := Sender;
            Current_Target := Target;
         end Send_Message;
         
         -- Przekazanie bez bufora: Serwer blokuje sie do momentu odebrania przez cel
         Users(Current_Target).Receive_Message(Current_Sender);
      end loop;

      -- Wyslanie sygnalu zamykajacego do wszystkich
      for I in 1 .. N loop
         Users(I).Shutdown;
      end loop;
   end Server;

   -- Cialo Uzytkownika
   task body User is
      My_ID : Integer;
      Received_Count : Integer := 0;
      Sent_Count : Integer := 0;
      Active : Boolean := True;

      subtype Target_Range is Integer range 1 .. N;
      package Random_Target is new Ada.Numerics.Discrete_Random(Target_Range);
      use Random_Target;
      Gen : Generator;
      Target : Integer;
   begin
      accept Start(ID : Integer) do
         My_ID := ID;
      end Start;
      Reset(Gen);

      while Active loop
         select
            -- Priorytet: bycie gotowym na odbior wiadomosci
            accept Receive_Message(Sender : in Integer) do
               Received_Count := Received_Count + 1;
               Put_Line("Uzytkownik" & Integer'Image(My_ID) & " <- OTRZYMAL od" & Integer'Image(Sender));
            end Receive_Message;
         or
            -- Priorytet: odebranie sygnalu zakonczenia
            accept Shutdown do
               Active := False;
            end Shutdown;
         else
            -- Jezeli nikt nam nic nie wysyla, to my probujemy wyslac
            if Sent_Count < M then
               Target := Random(Gen);
               select
                  -- Warunkowe wyslanie (Try-Send) do serwera
                  Server.Send_Message(My_ID, Target);
                  Sent_Count := Sent_Count + 1;
                  Put_Line("Uzytkownik" & Integer'Image(My_ID) & " -> WYSLAL do" & Integer'Image(Target));
               else
                  -- Serwer jest zajety (przetwarza cos innego) - unikamy zakleszczenia
                  delay 0.005;
               end select;
            else
               -- Skonczylismy wysylac, po prostu odczekujemy cykl
               delay 0.005;
            end if;
         end select;
      end loop;

      Put_Line("=== Uzytkownik" & Integer'Image(My_ID) & " odebral lacznie: " & Integer'Image(Received_Count) & " komunikatow ===");
   end User;

begin
   for I in 1 .. N loop
      Users(I).Start(I);
   end loop;
end Star_Network;