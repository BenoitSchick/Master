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

entity custom_instr_grayscale is
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

architecture rtl of custom_instr_grayscale is


    signal red_s, green_s, blue_s   					: unsigned(15 downto 0);
	signal red_weight_s, green_weight_s, blue_weight_s  : unsigned(15 downto 0);
	signal gray_s                   					: unsigned(15 downto 0);

begin
	

	
    red_s   <= unsigned(("00000000000" & dataa(15 downto 11)) and (x"1F")) sll 3;
    green_s <= unsigned(("00000" & dataa(15 downto 5)) and x"3F") sll 2;
    blue_s  <= unsigned(dataa(15 downto 0) and x"1F") sll 3;
	
	red_weight_s 	<= (red_s sll 4) + (red_s sll 2) + red_s; 		--21 = 16+4+1
	green_weight_s 	<= (green_s sll 6) + (green_s sll 3); 			--72 = 64 + 8
	blue_weight_s	<= (blue_s sll 2) + (blue_s sll 1) + blue_s; 	--7 = 4+2+1
	
	
	
	gray_s <= (red_weight_s + green_weight_s + blue_weight_s) srl 7;
	result <= (31 downto 8 => '0') & std_logic_vector(gray_s)(7 downto 0); --gray avec intensite entre 0 et 255 --> cast en 8 bits !

end architecture rtl;
