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

entity custom_instr_grayscale_multiBit is
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

architecture rtl of custom_instr_grayscale_multiBit is

    --Conversion RGB (16bits) en Grayscale (8bits)
    function rgb_to_grayscale(rgb_pixel : unsigned(15 downto 0)) return unsigned is
        variable  r_s, g_s, b_s                       : unsigned(15 downto 0);
        variable  r_weight_s, g_weight_s, b_weight_s  : unsigned(15 downto 0);
        variable  gray_s                              : unsigned(15 downto 0);
    begin
        --extraction des couleurs r,g,b
        r_s         := unsigned(("00000000000"  & rgb_pixel(15 downto 11))  and (x"001F")) sll 3;
        g_s         := unsigned(("00000"        & rgb_pixel(15 downto 5))   and x"003F")   sll 2;
        b_s         := unsigned(                  rgb_pixel(15 downto 0)    and x"001F")   sll 3;

        --ponderation des couleurs r,g,b
        r_weight_s 	:= (r_s sll 4) + (r_s sll 2) + r_s; 	--21 = 16+4+1
	    g_weight_s 	:= (g_s sll 6) + (g_s sll 3); 			--72 = 64 + 8
	    b_weight_s  := (b_s sll 2) + (b_s sll 1) + b_s; 	--7 = 4+2+1

        gray_s      := (r_weight_s + g_weight_s + b_weight_s) srl 7;

        return gray_s(7 downto 0); -- intensite entre 0 et 255 (8bits)
    end function;


    -- 4 pixels RGB de 16bits
    signal pix0, pix1, pix2, pix3 : unsigned(15 downto 0);
    -- 4 pixels grayscale de 8bits
    signal g0, g1, g2, g3 : unsigned(7 downto 0);

begin

    --Ordre depend du code C 
    pix0    <= unsigned(dataa(31 downto 16));
    pix1    <= unsigned(dataa(15 downto 0));
    pix2    <= unsigned(datab(31 downto 16));
    pix3    <= unsigned(datab(15 downto 0));

    g0      <= rgb_to_grayscale(pix0);
    g1      <= rgb_to_grayscale(pix1);
    g2      <= rgb_to_grayscale(pix2);
    g3      <= rgb_to_grayscale(pix3);
	
    result  <= std_logic_vector(g3 & g2 & g1 & g0); 

end architecture rtl;