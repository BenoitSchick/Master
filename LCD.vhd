library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


ENTITY LCD_controller IS
	PORT (
	
		-- Avalon Slave Interface
        Clk             : in	std_logic;
        nReset	        : in	std_logic;

        Address         : in	std_logic;	-- Selection du registre (cmd ou data)
		ChipSelect		: in	std_logic;						-- Si '1' processeur initie transaction avec LCD
        WriteData       : in	std_logic_vector(15 downto 0);
        ReadData        : out	std_logic_vector(15 downto 0);	-- pas utilise
        Write           : in	std_logic;
        Read            : in	std_logic;						-- pas utilise
        waitrequest     : out	std_logic;						-- Permet de bloquer le processeur pendant l'écriture du LCD

        -- LCD Controller interface
		
        --LCD_RESETn      : out	std_logic;
        --LCD_CSn         : out	std_logic;
        LCD_D_Cn        : out	std_logic;
        LCD_WRn         : out	std_logic;
        LCD_RDn         : out	std_logic;
		--IM0				: out	std_logic;
        LCD_DATA        : out	std_logic_vector(15 downto 0)
    );
End LCD_controller;

ARCHITECTURE arch of LCD_controller IS

	type lcd_state is (START, WRITE_1, WRITE_2, WRITE_3);
    signal current_state_s, next_state_s : lcd_state;
		
BEGIN

	
	process(Clk, nReset)
    begin
		if nReset = '0' then
            current_state_s <= START;			
        elsif rising_edge(Clk) then
			current_state_s <= next_state_s;
			
		end if;
	end process;
	
	
	
	--Process to define the future state_s
	process(current_state_s, Write, ChipSelect)
	begin
		case current_state_s is
			when START		=>
				if (ChipSelect = '1' and Write = '1') then    --ETAT IDLE MANQUANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					next_state_s <= WRITE_1;
				else
					next_state_s <= START;
				end if;
			when WRITE_1	=>	next_state_s <= WRITE_2;
			when WRITE_2	=>	next_state_s <= WRITE_3;
			when WRITE_3	=>	next_state_s <= START;
			when others 	=>	next_state_s <= START;
		end case;
	end process;
	
	
	--Process to affect the outputs
	process(current_state_s, Address, Write, ChipSelect) --Mealy
	begin
		
		--LCD_D_Cn	<= '0';
		LCD_WRn		<= '1';
		--LCD_DATA	<= WriteData;
		
		case current_state_s is
			when START		=>
				LCD_D_Cn <= Address;	--write cmd or data
				LCD_DATA <= WriteData;
				LCD_WRn  <= '0';
			when WRITE_1	=>	LCD_WRn  <= '1';	--sequence LCD_WRn for writing : ...0011...
			when WRITE_2	=>	LCD_WRn  <= '1';
			when WRITE_3	=>	LCD_WRn  <= '1';
			when others 	=>	LCD_WRn  <= '1';
		end case;
	end process;
	
	LCD_RDn		<= '1'; -- forced to 1
	waitrequest	<= '0' when current_state_s = START else '1'; -- block avalon bus ('1')
	
	
END arch;