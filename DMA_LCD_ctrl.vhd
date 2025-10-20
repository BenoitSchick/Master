
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity DMA_LCD_ctrl is
	port (
		clk               		: in    std_logic;
		reset             		: in    std_logic;         
		-- master interface
 		master_address	  		: out  std_logic_vector(31 downto 0);
		master_read	  			: out std_logic;
		master_readdata	  		: in std_logic_vector(15 downto 0) ;
		master_waitrequest 		: in std_logic;
		-- IRQ generation
		end_of_transaction_irq 	: out std_logic;		
		-- slave interface 
		avalon_address    		: in    std_logic_vector(2 downto 0);
		avalon_cs         		: in    std_logic;  
		avalon_wr         		: in    std_logic;  
		avalon_write_data 		: in    std_logic_vector(31 downto 0);
		avalon_rd         		: in    std_logic;  
		avalon_read_data  		: out    std_logic_vector(31 downto 0);
		-- LCD interface
		LCD_data          		: out std_logic_vector(15 downto 0);
		LCD_CS_n	  			: out    std_logic;		
		LCD_WR_n	  			: out    std_logic;				
		LCD_D_C_n	  			: out    std_logic 
	);
end entity DMA_LCD_ctrl;

architecture rtl of DMA_LCD_ctrl is


	--FSM states
	type lcd_state is (IDLE_LCD, START_LCD, WRITE_LCD);
	type dma_state is (IDLE, WAIT_MEM, READ_MEM, UPDATE_MEM, WRITE_DMA_LCD, END_TR);

	-- Signals
	signal lcd_state_s 		: lcd_state;
	signal dma_state_s 		: dma_state;
	signal pointer_img_s	: std_logic_vector(31 downto 0);
	signal lcd_data_s		: std_logic_vector(15 downto 0);
	signal lcd_count_wr_s	: unsigned(2 downto 0);
	--signal counter_lcd_s	: unsigned(2 downto 0);
	

	--DMA registers
	signal reg_pointer_img 	: std_logic_vector(31 downto 0);
	signal reg_size_img		: std_logic_vector(31 downto 0);
	signal reg_control_reg	: std_logic_vector(31 downto 0);
	
	

	--signal current_state_s, next_state_s : dma_state;
	
	--constants
	constant START_BIT 		: INTEGER := 0;
	constant IRQ_ACK_BIT 	: INTEGER := 2;
	constant NB_CYCLE_WR	: INTEGER := 4;		-- number of clock cycle to write to the LCD
	constant OFFSET_PTR		: INTEGER := 2;		-- offset applied to the image pointer
	

begin
-------- register model (a proposal) -----------
-- 000 write command to LCD
-- 001 write data to LCD
-- 010 write pointer of the image to copy
-- 011 write size of the image to copy
-- 100 control register 
--	bit 0 => start transfer 
--	bit 1 => lcd command 
--	bit 2 => IRQ ack 

	
	
	
	-- Registry write (slave interface)		 
	writeReg : process(clk, reset)
	begin
		if reset = '1' then
			lcd_data_s		<= (others => '0');
			pointer_img_s	<= (others => '0');
			reg_pointer_img <= (others => '0');
			reg_size_img	<= (others => '0');
			reg_control_reg	<= (others => '0');
			LCD_D_C_n 		<= '1';
			LCD_CS_n		<= '1';

		elsif rising_edge(clk) then
			if avalon_cs = '1' and avalon_wr = '1' then
				case avalon_address is 
					when "000" => 
						lcd_data_s	<= avalon_write_data;		--> execute by initLCD() function
						LCD_D_C_n 	<= '0'; -- cmd LCD
						LCD_CS_n	<= '0';
					when "001" => 
						lcd_data_s	<= avalon_write_data;
						LCD_CS_n	<= '0';
					when "010" => 
						reg_pointer_img <= avalon_write_data;	--> execute by initDMA() function
						LCD_CS_n	<= '0';
					when "011" => 
						reg_size_img	<= avalon_write_data;
						LCD_CS_n	<= '0';
					when "100" => 
						reg_control_reg	<= avalon_write_data;
						LCD_CS_n	<= '0';
					when others => null;
				end case;
			end if;
		end if;
	end process;


	-- Registry read (slave interface)
	readReg : process(clk, reset)
	begin
		if reset = '1' then
			avalon_read_data <= (others => '0');
		elsif rising_edge(clk) then
			if avalon_cs = '1' and avalon_rd = '1' then
				case avalon_address is
					when "010" 	=> avalon_read_data <= reg_pointer_img;
					when "011" 	=> avalon_read_data <= reg_size_img;
					when "100" 	=> avalon_read_data <= reg_control_reg;
					when others => avalon_read_data <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	
	--########################################################
	--##	Finite State Machine // Slave Interface : LCD	##
	--######################################################## 


	stateLCD_FSM : process(clk, reset)
	begin
		if reset = '1' then
			lcd_count_wr_s 	<= (others => '0');
			lcd_state_s		<= IDLE_LCD;

		elsif rising_edge(clk) then
			case lcd_state_s is
				when IDLE_LCD	=>
					if (avalon_cs = '1' and avalon_wr = '1' and (avalon_address = "000" or avalon_address = "001")) then -- Write cmd/data directly in LCD
						lcd_state_s	<= START_LCD;
					else
						lcd_state_s <= IDLE_LCD;
					end if;
				when START_LCD	=>
					lcd_state_s		<= WRITE_LCD;
				when WRITE_LCD	=>
					if lcd_count_wr_s < NB_CYCLE_WR then
						lcd_count_wr_s	<= lcd_count_wr_s + 1;
						lcd_state_s		<= WRITE_LCD;
					else
						lcd_count_wr_s	<= (others => '0');
						lcd_state_s		<= IDLE_LCD;
					end if;
			end case;
		end if;
	end process;

	outputLCD_FSM : process(lcd_state_s, avalon_address, avalon_wr, avalon_cs)
	begin
		case lcd_state_s is
			when IDLE_LCD	=> LCD_WR_n	<= '1';
			when START_LCD	=>
				LCD_WR_n	<= '0';
				--LCD_data	<= lcd_data_s;
			when WRITE_LCD	=> LCD_WR_n <= '1';
		end case;
	end process;



	--########################################################
	--##	Finite State Machine // Master Interface : DMA	##
	--######################################################## 


	stateDMA_FSM : process(clk, reset)
	begin
		if reset = '1' then
			dma_state_s		<= IDLE;
			pointer_img_s 	<= reg_pointer_img;
		elsif rising_edge(clk) then
			case dma_state_s is
				when IDLE			=>
					if reg_control_reg(START_BIT) = '1' then	-- Start transfer image to DMA
						dma_state_s	<= WAIT_MEM;
					else
						dma_state_s <= IDLE;
					end if;
				when WAIT_MEM		=>
					if master_waitrequest = '0' then
						dma_state_s	<= READ_MEM;
					else
						dma_state_s	<= WAIT_MEM;
					end if;
				when READ_MEM		=>
					dma_state_s		<= UPDATE_MEM;
				when UPDATE_MEM		=>
					pointer_img_s	<= std_logic_vector(unsigned(pointer_img_s) + to_unsigned(OFFSET_PTR, pointer_img_s'length));
					if (unsigned(reg_pointer_img) + unsigned(reg_size_img)) <= unsigned(pointer_img_s) then
						pointer_img_s <= (others => '0');
						dma_state_s	<= END_TR;
					else
						dma_state_s	<= WRITE_DMA_LCD;
					end if;
				when WRITE_DMA_LCD	=>
					dma_state_s		<= WAIT_MEM;
				when END_TR			=>
					if reg_control_reg(IRQ_ACK_BIT) = '1' then
						dma_state_s	<= IDLE;
					else
						dma_state_s	<= END_TR;
					end if;
				when others => null;
			end case;
		end if;
	end process;

	outputDMA_FSM : process(dma_state_s, pointer_img_s, master_readdata)
	begin

		master_read <= '0';
		LCD_WR_n	<= '1';

		case dma_state_s is
			when IDLE			=>
				end_of_transaction_irq <= '0';
			when WAIT_MEM		=>
				master_address	<= pointer_img_s;
			when READ_MEM		=>
				master_read		<= '1';
				lcd_data_s		<= master_readdata;
			when UPDATE_MEM		=>
			when WRITE_DMA_LCD	=>
				LCD_WR_n		<= '0';		-- write pixel image to LCD
			when END_TR			=>
				end_of_transaction_irq <= '1';
			when others => null;
		end case;
	end process;


	-- Asynchronous writing (LCD + DMA) --> LCD is written only when wr_n = 0
	LCD_data	<= lcd_data_s;

end architecture rtl; 

















--	process(clk, reset)
--    begin
--		if reset = '1' then
--            current_state_s <= IDLE;	
--			counter_lcd_s	<= (others => '0');
--        elsif rising_edge(clk) then
--			current_state_s <= next_state_s;
--		end if;
--	end process;
--	
--	
--	-- Process to determine the next state
--	nextState : process(current_state_s, reg_control_reg, master_waitrequest, master_readdata, avalon_address, avalon_cs, avalon_wr)
--	begin
--		case current_state_s is 
--			when IDLE 		=>
--				if reg_control_reg(START_BIT) = '1' then						-- Start transfer image to DMA
--					next_state_s	<= WAIT_MEM;								
--				elsif (avalon_address = "000" or avalon_address = "001") and avalon_cs = '1' and avalon_wr = '1' then		-- Execute fonction InitLCD()
--					next_state_s	<= START_LCD;
--				else
--					next_state_s 	<= IDLE;
--				end if;
--			when WAIT_MEM 	=>
--				if master_waitrequest = '0' then
--					next_state_s 	<= READ_MEM;
--				else
--					next_state_s 	<= WAIT_MEM;
--				end if;
--			when READ_MEM 	=> 
--				next_state_s <= UPDATE_MEM;
--			when UPDATE_MEM =>
--				if (unsigned(reg_pointer_img) + unsigned(reg_size_img)) <= unsigned(pointer_img_s) then
--					next_state_s <= END_TR;
--				else
--					next_state_s <= START_LCD;
--				end if;
--			when START_LCD 	=> 
--				if (avalon_cs = '1' and avalon_wr = '1') then
--					next_state_s <= WRITE_LCD;
--				else
--					next_state_s <= START_LCD;
--				end if;
--			when WRITE_LCD	=> 
--				if counter_lcd_s < 4 then
--					counter_lcd_s <= counter_lcd_s + 1;
--					next_state_s <= WRITE_LCD;
--				else
--					counter_lcd_s <= (others => '0');
--					next_state_s <= WAIT_MEM;
--				end if;
--			when END_TR 	=>
--				if reg_control_reg(IRQ_ACK_BIT) = '1' then
--					next_state_s	<= IDLE;
--				else
--					next_state_s	<= END_TR;
--				end if;
--		end case;
--	end process;
--	
--
--	-- Process to affect the outputs
--	process(current_state_s, avalon_address)
--	begin
--
--		LCD_WR_n	<= '1';
--
--		case current_state_s is 
--			when IDLE 		=>
--				
--			when WAIT_MEM 	=>
--			when READ_MEM 	=>
--				lcd_data_s <= master_readdata;
--			when UPDATE_MEM =>
--			when START_LCD 	=>
--				--LCD_D_Cn <= '1';	--write cmd or data
--				LCD_DATA <= lcd_data_s;
--				LCD_WR_n  <= '0';
--			when WRITE_LCD	=> LCD_WR_n  <= '1';
--			when END_TR 	=>
--		end case;
--	end process;