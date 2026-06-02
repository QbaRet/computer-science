with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;

procedure Dining_Philosophers is
   Num_Philosophers : constant Integer := 5;
   Num_Meals        : constant Integer := 3;

   protected type Fork is
      entry Grab;
      procedure Release;
   private
      Is_Free : Boolean := True;
   end Fork;

   protected type Butler is
      entry Enter;
      procedure Leave;
   private
      Count : Integer := 0;
   end Butler;

   protected body Fork is
      entry Grab when Is_Free is
      begin
         Is_Free := False;
      end Grab;
      
      procedure Release is
      begin
         Is_Free := True;
      end Release;
   end Fork;

   protected body Butler is
      entry Enter when Count < Num_Philosophers - 1 is
      begin
         Count := Count + 1;
      end Enter;

      procedure Leave is
      begin
         Count := Count - 1;
      end Leave;
   end Butler;

   Forks : array (1 .. Num_Philosophers) of Fork;
   Waiter : Butler;

   task type Philosopher is
      entry Start (My_ID : Integer; M : Integer);
   end Philosopher;

   task body Philosopher is
      ID, Meals_To_Eat, Meals, Fails, Left, Right : Integer;
      Gen : Generator;
   begin
      accept Start (My_ID : Integer; M : Integer) do
         ID := My_ID;
         Meals_To_Eat := M;
      end Start;
      
      Reset(Gen);
      Left  := ID;
      Right := (ID mod Num_Philosophers) + 1;
      Meals := 0;
      Fails := 0;

      while Meals < Meals_To_Eat loop
         Put_Line("Filozof " & Integer'Image(ID) & "mysli");
         delay Duration(Random(Gen) * 0.1);
         
         declare
            Entered : Boolean := False;
         begin
            select
               Waiter.Enter;
               Entered := True;
            or delay 0.01;
               Fails := Fails + 1;
            end select;

            if Entered then
               select
                  Forks(Left).Grab;
                  
                  select
                     Forks(Right).Grab;
                     
                     Put_Line("Filozof " & Integer'Image(ID) & "je");
                     delay Duration(Random(Gen) * 0.1);
                     Meals := Meals + 1;
                     
                     Forks(Right).Release;
                     Forks(Left).Release;
                     
                  or delay 0.01; 
                     Forks(Left).Release;
                     Fails := Fails + 1;
                  end select;
                  
               or delay 0.01; 
                  Fails := Fails + 1;
               end select;

               Waiter.Leave;
            end if;
         end;
      end loop;
      
      Put_Line("Filozof " & Integer'Image(ID) & " zakonczyl. Nieudane proby zjedzenia: " & Integer'Image(Fails));
   end Philosopher;

   Phils : array (1 .. Num_Philosophers) of Philosopher;
begin
   for I in 1 .. Num_Philosophers loop
      Phils(I).Start(I, Num_Meals);
   end loop;
end Dining_Philosophers;