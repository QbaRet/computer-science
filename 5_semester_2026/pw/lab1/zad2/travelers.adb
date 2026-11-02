--  Zadanie 2: na jednym polu może przebywać co najwyżej jeden podróżnik.
--  Każde pole pilnowane jest przez osobny wątek (Field_Task_Type).
--  Podejrzenie deadlocku wykrywane jest przez Timeout.
--
--  Uruchomienie:  ./travelers [Nr_Of_Travelers]   (domyślnie 15)

with Ada.Text_IO;               use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Ada.Real_Time;             use Ada.Real_Time;
with Ada.Characters.Handling;   use Ada.Characters.Handling;
with Ada.Command_Line;          use Ada.Command_Line;

procedure Travelers is

--  2D Board with torus topology

   Board_Width  : constant Integer := 15;  -- m
   Board_Height : constant Integer := 15;  -- n

--  Travelers moving on the board

   subtype Travelers_Count_Type is Integer range 1 .. Board_Width * Board_Height;

   --  constant; optional first argument changes the crowding of the board
   Nr_Of_Travelers : constant Travelers_Count_Type :=
     (if Argument_Count >= 1 then Integer'Value (Argument (1)) else 15);  -- k

   Min_Steps : constant Integer := 10;
   Max_Steps : constant Integer := 100;

   Min_Delay : constant Duration := 0.01;
   Max_Delay : constant Duration := 0.05;

   --  A traveler that waits longer than this for a free field
   --  suspects a deadlock (its neighbour had a few chances to move away).
   Timeout : constant Duration := 3 * Max_Delay;

--  Timing

   Start_Time : constant Time := Clock;  -- global starting time

--  Types, procedures and functions

   --  Positions on the board
   type Position_Type is record
      X : Integer range 0 .. Board_Width - 1;
      Y : Integer range 0 .. Board_Height - 1;
   end record;

   --  elementary steps
   procedure Move_Down (Position : in out Position_Type) is
   begin
      Position.Y := (Position.Y + 1) mod Board_Height;
   end Move_Down;

   procedure Move_Up (Position : in out Position_Type) is
   begin
      Position.Y := (Position.Y + Board_Height - 1) mod Board_Height;
   end Move_Up;

   procedure Move_Right (Position : in out Position_Type) is
   begin
      Position.X := (Position.X + 1) mod Board_Width;
   end Move_Right;

   procedure Move_Left (Position : in out Position_Type) is
   begin
      Position.X := (Position.X + Board_Width - 1) mod Board_Width;
   end Move_Left;

   --  traces of travelers
   type Trace_Type is record
      Time_Stamp : Duration;
      Id         : Integer;
      Position   : Position_Type;
      Symbol     : Character;
   end record;

   type Trace_Array_Type is array (0 .. Max_Steps) of Trace_Type;

   type Traces_Sequence_Type is record
      Last        : Integer := -1;
      Trace_Array : Trace_Array_Type;
   end record;

   procedure Print_Trace (Trace : Trace_Type) is
   begin
      Put_Line
        (Duration'Image (Trace.Time_Stamp) & " " &
         Integer'Image (Trace.Id) & " " &
         Integer'Image (Trace.Position.X) & " " &
         Integer'Image (Trace.Position.Y) & " " &
         (' ', Trace.Symbol));  -- print as string to avoid: '
   end Print_Trace;

   procedure Print_Traces (Traces : Traces_Sequence_Type) is
   begin
      for I in 0 .. Traces.Last loop
         Print_Trace (Traces.Trace_Array (I));
      end loop;
   end Print_Traces;

   --  task Printer collects and prints reports of traces
   task Printer is
      entry Report (Traces : Traces_Sequence_Type);
   end Printer;

   task body Printer is
   begin
      for I in 1 .. Nr_Of_Travelers loop
         accept Report (Traces : Traces_Sequence_Type) do
            Print_Traces (Traces);
         end Report;
      end loop;
   end Printer;

   --  Each field of the board is guarded by its own task:
   --  at most one traveler can be on the field at a time.
   task type Field_Task_Type is
      entry Enter;
      entry Leave;
   end Field_Task_Type;

   task body Field_Task_Type is
      Occupied : Boolean := False;
   begin
      loop
         select
            when not Occupied =>
               accept Enter do
                  Occupied := True;
               end Enter;
         or
            when Occupied =>
               accept Leave do
                  Occupied := False;
               end Leave;
         or
            terminate;
         end select;
      end loop;
   end Field_Task_Type;

   Board : array (0 .. Board_Width - 1, 0 .. Board_Height - 1)
     of Field_Task_Type;

   type Traveler_Type is record
      Id       : Integer;
      Symbol   : Character;
      Position : Position_Type;
   end record;

   task type Traveler_Task_Type is
      entry Init (Id : Integer; Seed : Integer; Symbol : Character);
      entry Start;
   end Traveler_Task_Type;

   task body Traveler_Task_Type is
      G            : Generator;
      Traveler     : Traveler_Type;
      Nr_Of_Steps  : Integer;
      Traces       : Traces_Sequence_Type;
      New_Position : Position_Type;

      procedure Store_Trace is
      begin
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array (Traces.Last) :=
           (Time_Stamp => To_Duration (Clock - Start_Time),
            Id         => Traveler.Id,
            Position   => Traveler.Position,
            Symbol     => Traveler.Symbol);
      end Store_Trace;

      function Random_Neighbour return Position_Type is
         Position : Position_Type := Traveler.Position;
      begin
         case Integer (Float'Floor (4.0 * Random (G))) mod 4 is
            when 0      => Move_Up (Position);
            when 1      => Move_Down (Position);
            when 2      => Move_Left (Position);
            when others => Move_Right (Position);
         end case;
         return Position;
      end Random_Neighbour;

   begin
      accept Init (Id : Integer; Seed : Integer; Symbol : Character) do
         Reset (G, Seed);
         Traveler.Id := Id;
         Traveler.Symbol := Symbol;
         --  Random initial position on a free field:
         loop
            Traveler.Position :=
              (X => Integer (Float'Floor (Float (Board_Width) * Random (G)))
                      mod Board_Width,
               Y => Integer (Float'Floor (Float (Board_Height) * Random (G)))
                      mod Board_Height);
            select
               Board (Traveler.Position.X, Traveler.Position.Y).Enter;
               exit;
            or
               delay 0.001;  -- field occupied, draw another one
            end select;
         end loop;
         Store_Trace;  -- store starting position
         --  Number of steps to be made by the traveler
         Nr_Of_Steps := Integer'Min
           (Max_Steps,
            Min_Steps + Integer (Float'Floor
              (Float (Max_Steps - Min_Steps + 1) * Random (G))));
      end Init;

      --  wait for initialisations of the remaining tasks:
      accept Start;

      for Step in 1 .. Nr_Of_Steps loop
         delay Min_Delay + (Max_Delay - Min_Delay) * Duration (Random (G));
         New_Position := Random_Neighbour;
         select
            Board (New_Position.X, New_Position.Y).Enter;
            Board (Traveler.Position.X, Traveler.Position.Y).Leave;
            Traveler.Position := New_Position;
            Store_Trace;
         or
            delay Timeout;
            --  suspected deadlock: leave the last trace and stop
            Traveler.Symbol := To_Lower (Traveler.Symbol);
            Store_Trace;
            exit;
         end select;
      end loop;

      Printer.Report (Traces);
   end Traveler_Task_Type;

--  local for main task

   Travel_Tasks : array (0 .. Nr_Of_Travelers - 1) of Traveler_Task_Type;
   Seed_Gen     : Generator;
   Symbol       : Character;

begin

   --  Print the line with the parameters needed for display script:
   Put_Line
     ("-1 " &
      Integer'Image (Nr_Of_Travelers) & " " &
      Integer'Image (Board_Width) & " " &
      Integer'Image (Board_Height));

   Reset (Seed_Gen);  -- time dependent

   --  init travelers tasks
   for I in Travel_Tasks'Range loop
      Symbol := Character'Val (Character'Pos ('A') + I mod 26);
      Travel_Tasks (I).Init
        (I, Integer (Float'Floor (1.0E6 * Random (Seed_Gen))), Symbol);
   end loop;

   --  start travelers tasks
   for I in Travel_Tasks'Range loop
      Travel_Tasks (I).Start;
   end loop;

end Travelers;
