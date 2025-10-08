library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


ENTITY ParallelPort IS
	PORT(
		-- Avalon interfaces signals
		
		Clk 			: IN std_logic;
		nReset 			: IN std_logic;
		Address 		: IN std_logic_vector (2 DOWNTO 0);		-- Pour choisir le registre (RegDir, RegPort, RegPin, RegSet, RegClr)
		ChipSelect 		: IN std_logic;							-- si '0' le module ParallelPort ignore les operations sur le bus Avalon
		Read 			: IN std_logic;							
		Write 			: IN std_logic;
		ReadData		: OUT std_logic_vector (31 DOWNTO 0);
		WriteData 		: IN std_logic_vector (31 DOWNTO 0);
		
		-- Parallel Port external interface
		ParPort 		: INOUT std_logic_vector (31 DOWNTO 0)
);
End ParallelPort;

ARCHITECTURE comp OF ParallelPort IS

	-- signals for register access
	signal iRegDir : std_logic_vector (31 DOWNTO 0);	-- Configurer I/O 
	signal iRegPort: std_logic_vector (31 DOWNTO 0);	-- Lire/Ecrire dans ParPort selon la config I/O
	signal iRegPin : std_logic_vector (31 DOWNTO 0);	-- Lire dans ParPort
	
BEGIN

	-- REGISTER ACCESS (WRITE)
	
	pRegWr: process(Clk, nReset)
	BEGIN
		if nReset = '0' then
			iRegDir 	<= (others => '0');
			iRegPort 	<= (others => '0');
		elsif rising_edge(Clk) then
			if ChipSelect = '1' and Write = '1' then	-- Write Cycle
				case Address(2 downto 0) is
					when "000" => 	iRegDir <= WriteData;
					--when "001" =>
					when "010" =>	iRegPort <= WriteData;
					when "011" =>	iRegPort <= iRegPort or WriteData;		 --Set
					when "100" =>	iRegPort <= iRegPort and not(WriteData); --Clr
					when others => null;
				end case;
			end if;
		end if;
	end process pRegWr;
	
	
	-- REGISTER ACCESS (READ)
	
	-- Read from registers with wait 0
	--ReadData <= iRegDir  when Address = "000" else	-- lit si pin I/O
	--			iRegPin  when Address = "001" else	-- lit l'état des pins
	--			iRegPort when Address = "010" else	-- lit l'état des pins
	--			"00000000";
	
	-- Read Process from registers with wait 1
	pRegRd: process(Clk)
	BEGIN
		if rising_edge(Clk) then
			ReadData <= (others => '0'); 
			if ChipSelect = '1' and Read = '1' then		-- Read Cycle
				case Address(2 downto 0) is
					when "000" => 	ReadData <= iRegDir;	-- lit si pin I/O
					when "001" =>	ReadData <= ParPort;--iRegPin;		-- lit directement l'état du port externe ParPort
					when "010" =>	ReadData <= iRegPort;	-- lit ce qu'on a ecrit dans ParPort
					when others => null;
				end case;
			end if;
		end if;
	end process pRegRd;
	
	
	-- EXTERNAL INTERFACE
	
	
	-- Parallel Port output value
	pPort: process(iRegDir, iRegPort)
	BEGIN
		for i in 0 to 31 loop
			if iRegDir(i) = '1' then
				ParPort(i) <= iRegPort(i);
			else
				ParPort(i) <= 'Z';
			end if;
		end loop;
	end process pPort;
	
	-- Parallel Port Input value
	--iRegPin <= ParPort;

END comp;
	
	