with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Discrete_Random;

procedure Star_Network is
   N : constant Integer := 5; 
   M : constant Integer := 3; 

   task Server is
      entry Send_Message(Sender : in Integer; Target : in Integer);
   end Server;

   task type User is
      entry Start(ID : Integer);
      entry Receive_Message(Sender : in Integer);
      entry Shutdown;
   end User;

   Users : array (1 .. N) of User;

   task body Server is
      Current_Sender, Current_Target : Integer;
   begin
      for I in 1 .. N * M loop
         accept Send_Message(Sender : in Integer; Target : in Integer) do
            Current_Sender := Sender;
            Current_Target := Target;
         end Send_Message;
         
         Users(Current_Target).Receive_Message(Current_Sender);
      end loop;

      for I in 1 .. N loop
         Users(I).Shutdown;
      end loop;
   end Server;

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
            accept Receive_Message(Sender : in Integer) do
               Received_Count := Received_Count + 1;
               Put_Line("Uzytkownik" & Integer'Image(My_ID) & " otrzymal od" & Integer'Image(Sender));
            end Receive_Message;
         or
            accept Shutdown do
               Active := False;
            end Shutdown;
         else
            if Sent_Count < M then
               Target := Random(Gen);
               select
                  Server.Send_Message(My_ID, Target);
                  Sent_Count := Sent_Count + 1;
                  Put_Line("Uzytkownik" & Integer'Image(My_ID) & " wyslal do" & Integer'Image(Target));
               else
                  delay 0.005;
               end select;
            else
               delay 0.005;
            end if;
         end select;
      end loop;

      Put_Line(" Uzytkownik" & Integer'Image(My_ID) & " odebral lacznie: " & Integer'Image(Received_Count) & " komunikatow");
   end User;

begin
   for I in 1 .. N loop
      Users(I).Start(I);
   end loop;
end Star_Network;