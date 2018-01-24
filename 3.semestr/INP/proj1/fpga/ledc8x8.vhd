--autor: Petr Juda | 2BIT
--email: xjudap00@stud.fit.vutbr.cz

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

--Popis rozhrani vstupu a vystupu
entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
	SMCLK: in std_logic;
	RESET: in std_logic;
	ROW: out std_logic_vector(7 downto 0);
	LED: out std_logic_vector(7 downto 0)
);
end ledc8x8;

--Definice pouzitych signalu
architecture main of ledc8x8 is
    -- Sem doplnte definice vnitrnich signalu.
    signal ce :std_logic :='0';
    signal switch: std_logic :='0';
    signal main_counter : std_logic_vector(22 downto 0);
    signal row_shift : std_logic_vector(7 downto 0);
    signal active_led : std_logic_vector(7 downto 0);

begin

    -- Sem doplnte popis funkce obvodu (zakladni konstrukce VHDL jako napr.
    -- prirazeni signalu, multiplexory, dekodery, procesy...).
    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL UVEDENE NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.



    --Synchroni cyklicy citac pro snizeni na smclk/256
    mCounter: process (SMCLK, RESET)
    begin
    	if RESET = '1' then
    		main_counter <= "00000000000000000000000";
    	elsif (SMCLK'event) and (SMCLK = '1') then
    		--Povoleni clock enable (regulace rychlosti prepinani, když counter napočítá 255, povolí ce.)
    		if main_counter(7 downto 0) = "11111111" 
    			then
    				ce <= '1';
    			else
    				ce <= '0';
    		end if;
			main_counter <= main_counter + '1';
		end if;
	end process mCounter;

	switch <= main_counter(22);

	--Rotace na prepinani aktivnich radku.
	rShift: process(SMCLK, ce, RESET)
	begin
		if RESET = '1' then
    		row_shift <= "10000000";
    	elsif (SMCLK'event) and (SMCLK = '1') and (ce = '1') then
    		--pokud je povolovaci signal 1, posun se na dalsi radek.
    		case row_shift is
				when "00000001" => row_shift <= "10000000";
				when "00000010" => row_shift <= "00000001";
				when "00000100" => row_shift <= "00000010";
				when "00001000" => row_shift <= "00000100";
				when "00010000" => row_shift <= "00001000";
				when "00100000" => row_shift <= "00010000";
				when "01000000" => row_shift <= "00100000";
				when "10000000" => row_shift <= "01000000";
				when others => null;	
			end case;
		end if;
		--preved aktualni radek na vystup
		ROW <= row_shift;
	end process rShift;

	--Dekoder pro rozsviceni led na aktivnim radku
	lDec: process(SMCLK, active_led)
	begin
		if (SMCLK'event) and (SMCLK = '1') then
			--kvuli prepinani pismen
			if switch = '0' then
				--pismeno p
				case row_shift is
					when "00000001" => active_led <= "11110001";
					when "00000010" => active_led <= "11101101";
					when "00000100" => active_led <= "11101101";
					when "00001000" => active_led <= "11110001";
					when "00010000" => active_led <= "11111101";
					when "00100000" => active_led <= "11111101";
					when "01000000" => active_led <= "11111101";
					when "10000000" => active_led <= "11111111";
					when others => null;
				end case;
			else
				--pismeno j
				case row_shift is
					when "00000001" => active_led <= "11101111";
					when "00000010" => active_led <= "11101111";
					when "00000100" => active_led <= "11101111";
					when "00001000" => active_led <= "11101111";
					when "00010000" => active_led <= "11101111";
					when "00100000" => active_led <= "11101101";
					when "01000000" => active_led <= "11110011";
					when "10000000" => active_led <= "11111111";
					when others => null;
					end case;
			end if;
		end if;
		LED <= active_led;
	end process lDec;


end main;
