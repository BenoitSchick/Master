library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


ENTITY GPIO_LCD IS
	PORT(
		-- Avalon interfaces signals
		
		Clk 			: IN std_logic;
		nReset 			: IN std_logic;
		Address 		: IN std_logic_vector (2 DOWNTO 0);		-- Pour choisir le registre (RegDir, RegPort, RegPin, RegSet, RegClr)
		ChipSelect 		: IN std_logic;							-- si '0' le module ParallelPort ignore les operations sur le bus Avalon
		Read_LCD 			: IN std_logic;							
		Write_LCD 			: IN std_logic;
		ReadData_LCD		: OUT std_logic_vector (7 DOWNTO 0);
		WriteData_LCD 		: IN std_logic_vector (7 DOWNTO 0);
		
		-- Parallel Port external interface
		gpio_lcd_o 		: INOUT std_logic_vector (7 DOWNTO 0)
);
End GPIO_LCD;

ARCHITECTURE comp OF GPIO_LCD IS

	-- signals for register access
	signal iRegDir : std_logic_vector (7 DOWNTO 0);	-- Configurer I/O 
	signal iRegPort: std_logic_vector (7 DOWNTO 0);	-- Lire/Ecrire dans gpio_lcd_o selon la config I/O
	signal iRegPin : std_logic_vector (7 DOWNTO 0);	-- Lire dans gpio_lcd_o
	
BEGIN

	-- REGISTER ACCESS (Write_LCD)
	
	pRegWr: process(Clk, nReset)
	BEGIN
		if nReset = '0' then
			iRegDir 	<= (others => '0');
			iRegPort 	<= (others => '0');
		elsif rising_edge(Clk) then
			if ChipSelect = '1' and Write_LCD = '1' then	-- Write Cycle
				case Address(2 downto 0) is
					when "000" => 	iRegDir <= WriteData_LCD;
					--when "001" =>
					when "010" =>	iRegPort <= WriteData_LCD;
					when "011" =>	iRegPort <= iRegPort or WriteData_LCD;		 --Set
					when "100" =>	iRegPort <= iRegPort and not(WriteData_LCD); --Clr
					when others => null;
				end case;
			end if;
		end if;
	end process pRegWr;
		
	
	-- REGISTER ACCESS (READ)
	
	-- Read from registers with wait 0
	--ReadData_LCD <= iRegDir  when Address = "000" else	-- lit si pin I/O
	--			iRegPin  when Address = "001" else	-- lit l'état des pins
	--			iRegPort when Address = "010" else	-- lit l'état des pins
	--			"00000000";
	
	-- Read Process from registers with wait 1
	pRegRd: process(Clk)
	BEGIN
		if rising_edge(Clk) then
			ReadData_LCD <= (others => '0'); 
			if ChipSelect = '1' and Read_LCD = '1' then		-- Read Cycle
				case Address(2 downto 0) is
					when "000" => 	ReadData_LCD <= iRegDir;	-- lit si pin I/O
					when "001" =>	ReadData_LCD <= gpio_lcd_o;--iRegPin;		-- lit directement l'état du port externe gpio_lcd_o
					when "010" =>	ReadData_LCD <= iRegPort;	-- lit ce qu'on a ecrit dans gpio_lcd_o
					when others => null;
				end case;
			end if;
		end if;
	end process pRegRd;
	
	
	-- EXTERNAL INTERFACE
	
	
	-- Parallel Port output value
	pPort: process(iRegDir, iRegPort)
	BEGIN
		for i in 0 to 7 loop
			if iRegDir(i) = '1' then
				gpio_lcd_o(i) <= iRegPort(i);
			else
				gpio_lcd_o(i) <= 'Z';
			end if;
		end loop;
	end process pPort;
	
	-- Parallel Port Input value
	--iRegPin <= gpio_lcd_o;

END comp;
	
	