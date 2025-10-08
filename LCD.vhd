library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


ENTITY LCD_controller IS
	PORT (
		-- Avalon Slave Interface
        Clk             : in	std_logic;
        nReset	        : in	std_logic;

        Address         : in	std_logic_vector(1 downto 0);
		ChipSelect		: in	std_logic;
        WriteData       : in	std_logic_vector(15 downto 0);
        ReadData        : out	std_logic_vector(15 downto 0);
        Write           : in	std_logic;
        Read            : in	std_logic;
        waitrequest     : out	std_logic;

        -- LCD Controller interface
        LCD_RESETn      : out	std_logic;
        LCD_CSn         : out	std_logic;
        LCD_D_Cn        : out	std_logic;
        LCD_WRn         : out	std_logic;
        LCD_RDn         : out	std_logic;
        LCD_DATA        : out	std_logic_vector(15 downto 0)
    );
End LCD_controller;

ARCHITECTURE arch of LCD_controller IS

	type state_type is (IDLE, WRITE_LCD, WAIT_END);
    signal state : state_type := IDLE;
	signal next_state : state_type;
	
	-- signals for register access
	signal lcd_cmd_s	: std_logic_vector(15 downto 0);	-- Ecriture d'une commande
	signal lcd_data_s	: std_logic_vector(15 downto 0);	-- Ecriture d'une donnée sur LCD
	signal lcd_status_s	: std_logic;						-- Etat du LCD
	
BEGIN

	process(Clk, nReset)
    begin
        if nReset = '1' then
            state <= IDLE;
        elsif rising_edge(clk) then
            state <= next_state;
        end if;
    end process;
	
	
	process(state)
    begin
        -- valeurs par défaut
        next_state 	<= current_state;
        LCD_RESETn 	<= '0';
		LCD_CSn		<= '0';
		LCD_D_Cn	<= '0';
		LCD_WRn		<= '0';
		LCD_RDn		<= '0';
		LCD_DATA	<= (others => '0');

        case current_state is

            when IDLE =>
               
            when WRITE_LCD =>
                
            when WAIT_END =>
                
            when others =>
                

        end case;
    end process;
	
	
END arch