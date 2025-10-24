
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity DMA_LCD_interface is
	port (
		clk               		: in    std_logic;
		reset_n	             	: in    std_logic;         
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
		LCD_RDn					: out std_logic;			
		LCD_D_C_n	  			: out    std_logic 
	);
end entity DMA_LCD_interface;

architecture rtl of DMA_LCD_interface is


	--FSM states
	type lcd_state is (IDLE_LCD, S1, S2, S3, S4);
	type dma_state is (IDLE, WAIT_MEM, READ_MEM, UPDATE_MEM, WRITE_DMA_LCD, END_TR);

	-- Signals
	signal lcd_state_s 		: lcd_state;
	signal dma_state_s 		: dma_state;
	signal offset_img_s		: unsigned(31 downto 0);
	signal counter_offset_img_s	: unsigned(31 downto 0);
	signal lcd_data_s		: std_logic_vector(15 downto 0);
	signal lcd_data_avalon_s	: std_logic_vector(15 downto 0);
    signal lcd_data_dma_s		: std_logic_vector(15 downto 0);
	--signal lcd_data_sel_s		: std_logic;
	--signal lcd_wr_n_procL	: std_logic;
	--signal lcd_wr_n_procD	: std_logic;
	signal dma_done_s				: std_logic;
	signal transfer_done_s			: std_logic;
	signal start_transfert_s		: std_logic;
	signal ack_s					: std_logic;
	signal irq_s					: std_logic;

	--signal counter_lcd_s	: unsigned(2 downto 0);
	

	--DMA registers
	signal reg_pointer_img 	: std_logic_vector(31 downto 0);
	signal reg_size_img		: unsigned(31 downto 0);
	
	

	--signal current_state_s, next_state_s : dma_state;
	
	

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

	
	IRQ : process(clk, reset_n)
	begin
		if reset_n = '0' then
			irq_s <= '0';
		elsif rising_edge(clk) then
			if dma_done_s = '1' then
				irq_s <= '1';
			elsif ack_s = '1' then
				irq_s <= '0';
			end if;
		end if;	
	end process;

	end_of_transaction_irq <= irq_s;
	
	
	-- Register write (slave interface)		 
	writeReg : process(clk, reset_n)
	begin
		if reset_n = '0' then
			--lcd_data_dma_s	<= (others => '0');
			--lcd_data_avalon_s		<= (others => '0');
			--reg_pointer_img <= (others => '0');
			--reg_size_img	<= (others => '0');
			ack_s <= '0';
			--start_transfert_s <= '0';
			--LCD_D_C_n 		<= '1';
			LCD_CS_n		<= '1';
			
		elsif rising_edge(clk) then
			ack_s <= '0';
			if avalon_cs = '1' and avalon_wr = '1' then
				case avalon_address is 
					when "000" => 
					--	lcd_data_avalon_s	<= avalon_write_data(15 downto 0);		--> execute by initLCD() function
						LCD_D_C_n 	<= '0'; -- cmd LCD
						LCD_CS_n	<= '0';
					when "001" => 
					--	lcd_data_avalon_s	<= avalon_write_data(15 downto 0);
						LCD_D_C_n 	<= '1';
						LCD_CS_n	<= '0';
					when "010" => 
						--reg_pointer_img <= avalon_write_data;	--> execute by initDMA() function
						LCD_D_C_n 	<= '1';
						LCD_CS_n	<= '0';
					when "011" => 
						--reg_size_img	<= avalon_write_data;
						LCD_D_C_n 	<= '1';
						LCD_CS_n	<= '0';
					when "100" => 
						LCD_D_C_n 	<= '1';
						--if avalon_write_data(0) = '1' then
						--	start_transfert_s <= '1';
						--end if;

						if avalon_write_data(2) = '1' then
							ack_s <= '1';
						end if;

						LCD_CS_n	<= '0';

					when "110" =>
						LCD_CS_n <= avalon_write_data(0);
					when others => null;
				end case;
			end if;
		end if;
	end process;


	-- Register read (slave interface)
	--readReg : process(clk, reset_n)
	--begin
	--	if reset_n = '0' then
	--		avalon_read_data <= (others => '0');
	--	elsif rising_edge(clk) then
	--		if avalon_cs = '1' and avalon_rd = '1' then
	--			case avalon_address is
	--				when "010" 	=> avalon_read_data <= reg_pointer_img;
	--				when "011" 	=> avalon_read_data <= reg_size_img;
	--				--when "100" 	=> avalon_read_data <= reg_control_reg;
	--				when others => avalon_read_data <= (others => '0');
	--			end case;
	--		end if;
	--	end if;
	--end process;

	
	--########################################################
	--##	Finite State Machine // Slave Interface : LCD	##
	--######################################################## 


	stateLCD_FSM : process(clk, reset_n)
	begin
		if reset_n = '0' then
			lcd_state_s		<= IDLE_LCD;
			lcd_data_s		<= (others => '0');
			--LCD_DCn_s		<= '1';
			--lcd_data_s		<= (others => '0');

		elsif rising_edge(clk) then
			case lcd_state_s is
				when IDLE_LCD	=>
					if dma_state_s = WRITE_DMA_LCD then
						lcd_data_s <= lcd_data_dma_s;
						lcd_state_s	<= S1;
					end if;

					if avalon_cs = '1' and avalon_wr = '1' and (avalon_address = "000" or avalon_address = "001") then 
						lcd_state_s	<= S1;
						lcd_data_s <= avalon_write_data(15 downto 0);
					end if;
				when S1	=> lcd_state_s	<= S2;
				when S2	=> lcd_state_s	<= S3;
				when S3 => lcd_state_s	<= S4;
				when S4 => lcd_state_s	<= IDLE_LCD;
				when others => null;
			end case;
		end if;
	end process;

	outputLCD_FSM : process(lcd_state_s, avalon_wr, avalon_cs)
	begin
		
		LCD_WR_n <= '1';
		
			case lcd_state_s is
				when IDLE_LCD	=>
					transfer_done_s <= '0';
					LCD_WR_n <= '1';
				when S1			=>
					transfer_done_s <= '0';
					LCD_WR_n <= '0';
				when S2			=>
					transfer_done_s <= '0';
					LCD_WR_n <= '0';
				when S3			=>
					transfer_done_s <= '0';
					LCD_WR_n <= '1';
				when S4			=>
					transfer_done_s <= '1';
					LCD_WR_n <= '1';
				when others => null;
			end case;

	end process;

	LCD_data	<= lcd_data_s;
	LCD_RDn		<= '1';



	--########################################################
	--##	Finite State Machine // Master Interface : DMA	##
	--######################################################## 


	stateDMA_FSM : process(clk, reset_n)
	begin
		if reset_n = '0' then
			dma_state_s				<= IDLE;
			offset_img_s			<= (others => '0');
			counter_offset_img_s 	<= (others => '0');
			lcd_data_dma_s			<= (others => '0');
			reg_pointer_img <= (others => '0');
			reg_size_img	<= (others => '0');
			--ack_s <= '0';
			start_transfert_s <= '0';

			dma_done_s <= '0';

		elsif rising_edge(clk) then
			--ack_s <= '0';
			dma_done_s <= '0';
			case dma_state_s is
				when IDLE			=>

					if avalon_cs = '1' and avalon_wr = '1' then
						case avalon_address is
							when "010" => 
								reg_pointer_img <= avalon_write_data;
								
							when "011" => 
								reg_size_img	<= unsigned(avalon_write_data);
							when "100" => 
								if avalon_write_data(0) = '1' then
									start_transfert_s <= '1';
								end if;
							when others => null;
						end case;
					end if;



					if start_transfert_s = '1' then	-- Start transfer image to DMA
						dma_state_s	<= READ_MEM;
					else
						dma_state_s <= IDLE;
					end if;

				when READ_MEM		=>
					dma_state_s		<= WAIT_MEM;

				when WAIT_MEM		=>
					if master_waitrequest = '0' then
						dma_state_s	<= WRITE_DMA_LCD;
						lcd_data_dma_s	<= master_readdata;
					else
						dma_state_s	<= WAIT_MEM;
					end if;
				
				when WRITE_DMA_LCD	=>
					if transfer_done_s = '1' then
						dma_state_s		<= UPDATE_MEM;
					end if;

				when UPDATE_MEM		=>
					if counter_offset_img_s >= (reg_size_img - 1) then
						dma_state_s	<= END_TR;
					else
						if master_waitrequest = '1' then
							dma_state_s <= READ_MEM;
							counter_offset_img_s <= counter_offset_img_s + 1;
							offset_img_s	<= offset_img_s + 2;
						end if;
					end if;
				when END_TR			=>
					dma_done_s <= '1';
					if ack_s = '1' then
						dma_state_s <= IDLE;
						start_transfert_s <= '0';
					end if;

					--if ack_s = '1' then
					--	dma_state_s	<= IDLE;
					--	start_transfert_s <= '0';
					--else
					--	if avalon_cs = '1' and avalon_wr = '1' then 
					--		if avalon_address = "100" then
					--			ack_s <= avalon_write_data(2);
					--		end if;
					--	end if;
					--	dma_state_s	<= END_TR;
					--end if;
				when others => null;
			end case;
		end if;
	end process;

	outputDMA_FSM : process(dma_state_s, master_readdata)
	begin

		master_read 	<= '0';
		--end_of_transaction_irq <= '0';

		case dma_state_s is
			when IDLE			=>
				--end_of_transaction_irq <= '0';
			when READ_MEM		=>
				master_read		<= '1';
				master_address	<= std_logic_vector(unsigned(reg_pointer_img) + offset_img_s);
			when WAIT_MEM		=>
				master_read		<= '1';
			when WRITE_DMA_LCD	=>
			when UPDATE_MEM		=>
			--when END_TR			=>
			--	if ack_s = '0' then
			--		end_of_transaction_irq <= '1';
			--	end if;
			when others => null;
		end case;
	end process;
end architecture rtl; 