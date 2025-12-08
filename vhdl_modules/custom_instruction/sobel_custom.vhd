library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

--########################################
-- Combinational Custom Instruction Ports
--########################################

-- Input    //  dataa[31:0]     //  Required : no   //  Input operand to custom instruction
-- Input    //  datab[31:0]     //  Required : no   //  Input operand to custom instruction
-- Output   //  result[31:0]    //  Required : yes  //  Result of custom instruction

--#####################################
-- Multicycle Custom Instruction Ports
--#####################################

-- Input    //  clk             //  Required : yes  //  System Clock
-- Input    //  clk_en          //  Required : yes  //  Clock enable
-- Input    //  reset           //  Required : yes  //  Synchronous reset
-- Input    //  start           //  Required : no   //  Commands custom instruction logic to start execution
-- Output   //  done            //  Required : no   //  Custom instruction logic indicates to the processor that execution is complete
-- Input    //  dataa[31:0]     //  Required : no   //  Input operand to custom instruction
-- Input    //  datab[31:0]     //  Required : no   //  Input operand to custom instruction
-- Output   //  result[31:0]    //  Required : no   //  Result of custom instruction


--#############################
-- Extended Custom Instruction
--#############################

-- Input    //  n               // Required : yes   //  select multiple sub-operations in the same instruction (limits : 256)

entity custom_instr_sobel is
    port (
        --clk     : in  std_logic;
        --reset   : in  std_logic;
        dataa   : in  std_logic_vector(31 downto 0);
        datab   : in  std_logic_vector(31 downto 0);
        --n       : in  std_logic_vector(7 downto 0);
        --start   : in  std_logic;  -- NIOS II déclenche le début de l'instruction

        --done    : out std_logic;  -- On indique quand on termine l'instruction
        result  : out std_logic_vector(31 downto 0)
    );
end entity;

architecture rtl of custom_instr_sobel is

--nombre signe 9 bits : -256 à 255 !
signal row_ym1_xminus1 	: signed(8 downto 0); -- row_ym1[x-1]
signal row_ym1_xplus1  	: signed(8 downto 0); -- row_ym1[x+1]
signal row_y0_xminus1  	: signed(8 downto 0); -- row_y0[x-1]
signal row_y0_xplus1   	: signed(8 downto 0); -- row_y0[x+1]
signal row_yp1_xminus1 	: signed(8 downto 0); -- row_yp1[x-1]
signal row_yp1_xplus1  	: signed(8 downto 0); -- row_yp1[x+1]
signal row_ym1_x      	: signed(8 downto 0); -- row_ym1[x]
signal row_yp1_x      	: signed(8 downto 0); -- row_yp1[x]

signal gx_s				: signed(31 downto 0);
signal gx_abs_s			: signed(31 downto 0);
signal gy_s				: signed(31 downto 0);
signal gy_abs_s			: signed(31 downto 0);
signal sum_s			: signed(31 downto 0);
signal result_sobel_s	: signed(31 downto 0);	-- pixel avec intensite entre 0 et 255
constant TRESHOLD  		: integer := 128;

begin
-- chaque row contient 8 bits --> extendre à 9 bits : -256 à 255

-- Initialisation à partir de dataa
	row_ym1_xminus1 <= signed('0' & dataa(31 downto 24));
	row_ym1_xplus1  <= signed('0' & dataa(23 downto 16));
	row_y0_xminus1  <= signed('0' & dataa(15 downto 8));
	row_y0_xplus1   <= signed('0' & dataa(7 downto 0));

-- Initialisation à partir de datab
	row_yp1_xminus1 <= signed('0' & datab(31 downto 24));
	row_yp1_xplus1  <= signed('0' & datab(23 downto 16));
	row_ym1_x      	<= signed('0' & datab(15 downto 8));
	row_yp1_x      	<= signed('0' & datab(7 downto 0));

--Calcul de la convolution avec matrice de sobel
	gx_s 		<= 	resize((-row_ym1_xminus1 + row_ym1_xplus1
					- (row_y0_xminus1 sll 1) + (row_y0_xplus1 sll 1)
					- row_yp1_xminus1 + row_yp1_xplus1), 32);
					
	gy_s 		<= 	resize((row_ym1_xminus1 + (row_ym1_x sll 1) + row_ym1_xplus1
					- row_yp1_xminus1 - (row_yp1_x sll 1) - row_yp1_xplus1),32);

	gx_abs_s 	<= abs(gx_s); --abs retourne le meme type que l'argument
	gy_abs_s 	<= abs(gy_s);

	sum_s	 	<=	gx_abs_s + gy_abs_s; --gx_abs_s & gy_abs_s sont positif, on peut retourner en unsigned

	result	<= 	x"FFFFFFFF" when (sum_s >= to_signed(TRESHOLD, 32) ) else
				x"00000000";
end architecture rtl;
