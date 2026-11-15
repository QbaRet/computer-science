with Ada.Text_IO;               use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Ada.Real_Time;             use Ada.Real_Time;
with Ada.Characters.Handling;   use Ada.Characters.Handling;
with Ada.Command_Line;          use Ada.Command_Line;

procedure Travelers is
   Board_Width  : constant Integer := 15;
   Board_Height : constant Integer := 15;

   subtype Travelers_Count_Type is Integer range 1 .. Board_Width * Board_Height;

   Nr_Of_Travelers : constant Travelers_Count_Type :=
     (if Argument_Count >= 1 then Integer'Value (Argument (1)) else 15);

   Min_Steps : constant Integer := 10;
   Max_Steps : constant Integer := 100;

   Min_Delay : constant Duration := 0.01;
   Max_Delay : constant Duration := 0.05;

   Timeout : constant Duration := 3 * Max_Delay;

   -- Dzicy lokatorzy
   Nr_Of_Squatters   : constant Integer  := 10;
   Max_Appear_Delay  : constant Duration := 2.0;  -- pojawia sie w losowym momencie 0..2 s
   Min_Squatter_Life : constant Duration := 0.5;
   Max_Squatter_Life : constant Duration := 1.5;
   Max_Appear_Tries  : constant Integer  := 20;

   -- Id: podroznicy 0..k-1, potem dzicy lokatorzy
   Nr_Of_Ids : constant Integer := Nr_Of_Travelers + Nr_Of_Squatters;

   Start_Time : constant Time := Clock;

   -- Board_Width / Board_Height oznacza "poza plansza"
   type Position_Type is record
      X : Integer range 0 .. Board_Width;
      Y : Integer range 0 .. Board_Height;
   end record;

   Hidden : constant Position_Type := (X => Board_Width, Y => Board_Height);

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

   type Trace_Type is record
      Time_Stamp : Duration;
      Id         : Integer;
      Position   : Position_Type;
      Symbol     : Character;
   end record;

   type Trace_Array_Type is array (0 .. Max_Steps + 1) of Trace_Type;

   type Traces_Sequence_Type is record
      Last        : Integer := -1;
      Trace_Array : Trace_Array_Type;
   end record;

   procedure Store_Trace
     (Traces   : in out Traces_Sequence_Type;
      Id       : Integer;
      Position : Position_Type;
      Symbol   : Character) is
   begin
      if Traces.Last < Trace_Array_Type'Last then
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array (Traces.Last) :=
           (Time_Stamp => To_Duration (Clock - Start_Time),
            Id         => Id,
            Position   => Position,
            Symbol     => Symbol);
      end if;
   end Store_Trace;

   procedure Print_Trace (Trace : Trace_Type) is
   begin
      Put_Line
        (Duration'Image (Trace.Time_Stamp) & " " &
         Integer'Image (Trace.Id) & " " &
         Integer'Image (Trace.Position.X) & " " &
         Integer'Image (Trace.Position.Y) & " " &
         (' ', Trace.Symbol));
   end Print_Trace;

   procedure Print_Traces (Traces : Traces_Sequence_Type) is
   begin
      for I in 0 .. Traces.Last loop
         Print_Trace (Traces.Trace_Array (I));
      end loop;
   end Print_Traces;

   -- Dziki lokator: dane, ktore "mieszkaja" w watku pola i przechodza
   -- do sasiedniego pola, gdy lokator musi sie wyprowadzic.
   type Squatter_Type is record
      Id         : Integer;
      Symbol     : Character;
      Death_Time : Time;
      Traces     : Traces_Sequence_Type;
   end record;

   type Occupant_Type is (None, Traveler, Squatter);
   type Enter_Result_Type is (Entered, Blocked);

   task Printer is
      entry Report (Traces : Traces_Sequence_Type);
   end Printer;

   task type Field_Task_Type is
      entry Init (X, Y : Integer);
      entry Enter (From : Position_Type; Result : out Enter_Result_Type);
      entry Leave;
      entry Squatter_Arrive (S : Squatter_Type);
   end Field_Task_Type;

   Board : array (0 .. Board_Width - 1, 0 .. Board_Height - 1)
     of Field_Task_Type;

   task body Printer is
   begin
      -- kazdy podroznik i kazdy dziki lokator wysyla dokladnie jeden raport
      for I in 1 .. Nr_Of_Ids loop
         accept Report (Traces : Traces_Sequence_Type) do
            Print_Traces (Traces);
         end Report;
      end loop;
   end Printer;

   task body Field_Task_Type is
      My_Position : Position_Type;
      Occupant    : Occupant_Type := None;
      Sq          : Squatter_Type;   -- wazne tylko gdy Occupant = Squatter

      -- Proba przeniesienia lokatora na wolne sasiednie pole rozne od From.
      -- Wywolanie warunkowe (select ... else): jesli sasiad nie moze
      -- przyjac lokatora od razu, probujemy nastepnego - nigdy nie czekamy.
      procedure Move_Squatter_Away (From : Position_Type; Moved : out Boolean) is
         Neighbours : array (1 .. 4) of Position_Type := (others => My_Position);
      begin
         Move_Up (Neighbours (1));
         Move_Down (Neighbours (2));
         Move_Left (Neighbours (3));
         Move_Right (Neighbours (4));
         Moved := False;
         for N of Neighbours loop
            if N /= From then
               select
                  Board (N.X, N.Y).Squatter_Arrive (Sq);
                  Moved := True;
               else
                  null;
               end select;
               exit when Moved;
            end if;
         end loop;
      end Move_Squatter_Away;

      procedure Handle_Enter (From : Position_Type; Result : out Enter_Result_Type) is
         Moved : Boolean;
      begin
         if Occupant = Squatter then
            Move_Squatter_Away (From, Moved);
            if not Moved then
               Result := Blocked;   -- lokator nie ma dokad uciec
               return;
            end if;
         end if;
         Occupant := Traveler;
         Result := Entered;
      end Handle_Enter;

      procedure Handle_Arrive (S : Squatter_Type) is
      begin
         Sq := S;
         Store_Trace (Sq.Traces, Sq.Id, My_Position, Sq.Symbol);
         Occupant := Squatter;
      end Handle_Arrive;

      procedure Squatter_Dies is
      begin
         Store_Trace (Sq.Traces, Sq.Id, Hidden, Sq.Symbol);  -- znika z planszy
         Occupant := None;
         Printer.Report (Sq.Traces);
      end Squatter_Dies;

   begin
      accept Init (X, Y : Integer) do
         My_Position := (X => X, Y => Y);
      end Init;

      loop
         if Occupant = Squatter then
            -- Na polu mieszka lokator: czekamy na podroznika albo na koniec
            -- zycia lokatora. (Ada nie pozwala laczyc delay z terminate,
            -- dlatego sa dwa osobne select.)
            select
               accept Enter (From : Position_Type; Result : out Enter_Result_Type) do
                  Handle_Enter (From, Result);
               end Enter;
            or
               delay until Sq.Death_Time;
               Squatter_Dies;
            end select;
         else
            select
               when Occupant = None =>
                  accept Enter (From : Position_Type; Result : out Enter_Result_Type) do
                     Handle_Enter (From, Result);
                  end Enter;
            or
               when Occupant = Traveler =>
                  accept Leave do
                     Occupant := None;
                  end Leave;
            or
               when Occupant = None =>
                  accept Squatter_Arrive (S : Squatter_Type) do
                     Handle_Arrive (S);  -- slad zapisany jeszcze w trakcie spotkania
                  end Squatter_Arrive;
            or
               terminate;
            end select;
         end if;
      end loop;
   end Field_Task_Type;

   function Random_Position (G : Generator) return Position_Type is
   begin
      return
        (X => Integer (Float'Floor (Float (Board_Width) * Random (G))) mod Board_Width,
         Y => Integer (Float'Floor (Float (Board_Height) * Random (G))) mod Board_Height);
   end Random_Position;

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
      Old_Position : Position_Type;
      Result       : Enter_Result_Type;

      procedure Store_Trace is
      begin
         Store_Trace (Traces, Traveler.Id, Traveler.Position, Traveler.Symbol);
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
         loop
            Traveler.Position := Random_Position (G);
            select
               Board (Traveler.Position.X, Traveler.Position.Y).Enter
                 (Traveler.Position, Result);
               exit when Result = Entered;
            or
               delay 0.001;
            end select;
         end loop;
         Store_Trace;
         Nr_Of_Steps := Integer'Min
           (Max_Steps,
            Min_Steps + Integer (Float'Floor
              (Float (Max_Steps - Min_Steps + 1) * Random (G))));
      end Init;

      accept Start;

      for Step in 1 .. Nr_Of_Steps loop
         delay Min_Delay + (Max_Delay - Min_Delay) * Duration (Random (G));
         New_Position := Random_Neighbour;
         select
            Board (New_Position.X, New_Position.Y).Enter (Traveler.Position, Result);
            if Result = Entered then
               Old_Position := Traveler.Position;
               Traveler.Position := New_Position;
               Store_Trace;  -- przed Leave, zeby slad byl wczesniej niz kolejne wejscie
               Board (Old_Position.X, Old_Position.Y).Leave;
            end if;
            -- Blocked: lokator nie mial dokad uciec, w nastepnym kroku
            -- podroznik losuje kierunek od nowa
         or
            delay Timeout;
            Traveler.Symbol := To_Lower (Traveler.Symbol);
            Store_Trace;
            exit;
         end select;
      end loop;

      Printer.Report (Traces);
   end Traveler_Task_Type;

   -- Watek, ktory w losowym momencie stawia dzikiego lokatora na losowym
   -- wolnym polu. Dalej lokatorem zajmuja sie juz watki pol.
   task type Squatter_Spawner_Type is
      entry Init (Id : Integer; Seed : Integer; Symbol : Character);
      entry Start;
   end Squatter_Spawner_Type;

   task body Squatter_Spawner_Type is
      G        : Generator;
      Sq       : Squatter_Type;
      Position : Position_Type;
      Placed   : Boolean := False;
   begin
      accept Init (Id : Integer; Seed : Integer; Symbol : Character) do
         Reset (G, Seed);
         Sq.Id := Id;
         Sq.Symbol := Symbol;
      end Init;

      accept Start;

      delay Max_Appear_Delay * Duration (Random (G));

      for Try in 1 .. Max_Appear_Tries loop
         Position := Random_Position (G);
         Sq.Death_Time := Clock + To_Time_Span
           (Min_Squatter_Life +
            (Max_Squatter_Life - Min_Squatter_Life) * Duration (Random (G)));
         select
            Board (Position.X, Position.Y).Squatter_Arrive (Sq);
            Placed := True;
         else
            null;   -- pole zajete, losujemy inne
         end select;
         exit when Placed;
         delay 0.01;
      end loop;

      if not Placed then
         Printer.Report (Sq.Traces);  -- pusty raport, zeby Printer sie doliczyl
      end if;
   end Squatter_Spawner_Type;

   Travel_Tasks : array (0 .. Nr_Of_Travelers - 1) of Traveler_Task_Type;
   Spawners     : array (0 .. Nr_Of_Squatters - 1) of Squatter_Spawner_Type;
   Seed_Gen     : Generator;

begin
   Put_Line
     ("-1 " &
      Integer'Image (Nr_Of_Ids) & " " &
      Integer'Image (Board_Width) & " " &
      Integer'Image (Board_Height));

   for X in Board'Range (1) loop
      for Y in Board'Range (2) loop
         Board (X, Y).Init (X, Y);
      end loop;
   end loop;

   Reset (Seed_Gen);

   for I in Travel_Tasks'Range loop
      Travel_Tasks (I).Init
        (I, Integer (Float'Floor (1.0E6 * Random (Seed_Gen))),
         Character'Val (Character'Pos ('A') + I mod 26));
   end loop;

   for I in Spawners'Range loop
      Spawners (I).Init
        (Nr_Of_Travelers + I, Integer (Float'Floor (1.0E6 * Random (Seed_Gen))),
         Character'Val (Character'Pos ('0') + I mod 10));
   end loop;

   for I in Travel_Tasks'Range loop
      Travel_Tasks (I).Start;
   end loop;

   for I in Spawners'Range loop
      Spawners (I).Start;
   end loop;

end Travelers;
