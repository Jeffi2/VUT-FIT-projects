-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2016 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xjudap00@stud.fit.vutbr.cz | Petr Juda
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu
 signal pc_reg_inc : std_logic;
 signal pc_reg_dec : std_logic;
 signal pc_reg_data : std_logic_vector(11 downto 0);

 signal tmp_reg_data : std_logic_vector(7 downto 0);
 signal tmp_ld : std_logic;
 signal data_rdata_zero : std_logic;
 

 signal cnt_reg_data : std_logic_vector(7 downto 0);
 signal cnt_reg_inc : std_logic;
 signal cnt_reg_dec : std_logic;
 signal cnt_zero : std_logic;

 signal ptr_reg_data : std_logic_vector(9 downto 0);
 signal ptr_reg_inc : std_logic;
 signal ptr_reg_dec : std_logic;

 type instr_type is (inc_ptr, dec_ptr, inc_value, dec_value, whl_start, whl_end, print, load, tmp_store, tmp_send, halt, nop);
 signal i_dec_type : instr_type;

 signal sel : std_logic_vector(1 downto 0);

 --konecny automat
 type fsm_state is (
   sidle, decode, s_inc_ptr, s_dec_ptr, s_inc_value, s_dec_value, s_whl_start, s_whl_end, s_print, s_load, s_tmp_store, s_tmp_send, s_halt, s_nop,
   s_inc_value2, s_dec_value2, s_print1, s_load2, s_tmp_store2, s_whl_start2, s_whl_start3, s_whl_start4, s_whl_end2, s_whl_end3, s_whl_end4, s_whl_end5
   );

 signal pstate : fsm_state;
 signal nstate : fsm_state;

begin

--registr pc_reg
pc_reg: process (RESET, CLK)
begin
   if (RESET = '1') then
      pc_reg_data <= (others => '0');
      elsif (CLK'event) and (CLK = '1') then
         if (pc_reg_inc = '1') then
            pc_reg_data <= pc_reg_data + 1; 
         elsif (pc_reg_dec = '1') then
            pc_reg_data <= pc_reg_data - 1;
         end if ;
   end if ;
end process;
-- nahrejeme obsah registru
CODE_ADDR <= pc_reg_data;

--registr tmp_reg
tmp_reg: process (RESET, CLK, DATA_RDATA)
begin
   if (RESET = '1') then
      tmp_reg_data <= (others => '0');
      elsif (CLK'event) and (CLK = '1') then
         if (tmp_ld = '1') then
            tmp_reg_data <= DATA_RDATA;
         end if ;
      end if ;
   
end process;   
--if DATA_RDATA = 0 then data_rdata_zero is true
data_rdata_zero <= '1' when (DATA_RDATA = "00000000") else '0';


--registr ptr_reg
ptr_reg: process (RESET, CLK)
begin
   if (RESET = '1') then
      ptr_reg_data <= (others => '0');
      elsif (CLK'event) and (CLK = '1') then
         if (ptr_reg_inc='1') then
            --pokud je na konci, jdi na zacatek
            if (ptr_reg_data = "1111111111") then
               ptr_reg_data <= "0000000000";
            else
               -- jinak inc
               ptr_reg_data <= ptr_reg_data + 1;      
            end if ;
         elsif (ptr_reg_dec = '1') then
            -- pokud dec na zacatku jdi na konec
            if (ptr_reg_data = "0000000000") then
               ptr_reg_data <= "1111111111";
            else
               --jinak dec
               ptr_reg_data <= ptr_reg_data - 1;
            end if ;
         end if ;
 end if ; 
end process;
--nahrajeme obsah registru
DATA_ADDR <= ptr_reg_data;

--registr cnt_reg
cnt_reg: process (RESET, CLK)
begin
   if (RESET = '1') then
      cnt_reg_data <= (others => '0');
      elsif (CLK'event) and (CLK = '1') then
         if (cnt_reg_dec = '1') then
            cnt_reg_data <= cnt_reg_data - 1;
         elsif (cnt_reg_inc = '1') then
            cnt_reg_data <= cnt_reg_data + 1;
         end if ;
   end if ;
end process;
--if cnt_reg = 0 then cnt_zero is true
cnt_zero <= '1' when (cnt_reg_data = "00000000") else '0';

--decoder intrukci
dec: process (CODE_DATA)
begin
   case (CODE_DATA) is
         when X"3E" => i_dec_type <= inc_ptr; -- >
         when X"3C" => i_dec_type <= dec_ptr; -- <
         when X"2B" => i_dec_type <= inc_value; -- +
         when X"2D" => i_dec_type <= dec_value; -- -
         when X"5B" => i_dec_type <= whl_start; -- [
         when X"5D" => i_dec_type <= whl_end; --]
         when X"2E" => i_dec_type <= print; -- . 
         when X"2C" => i_dec_type <= load; -- ,
         when X"24" => i_dec_type <= tmp_store; -- $
         when X"21" => i_dec_type <= tmp_send; -- !
         when X"00" => i_dec_type <= halt; -- null
         when others => i_dec_type <= nop; -- comment
      end case;
end process;

--mux 
mux: process(sel, DATA_RDATA, tmp_reg_data, IN_DATA) 
begin
   case sel is
         when "00" => DATA_WDATA <= IN_DATA;
         when "01" => DATA_WDATA <= tmp_reg_data;
         when "10" => DATA_WDATA <= DATA_RDATA - "00000001";
         when "11" => DATA_WDATA <= DATA_RDATA + "00000001";
         when others =>
   end case;
end process;

--konecny automat present state
fsm_pstate: process (RESET, CLK)
begin
   if (RESET = '1') then
      pstate <= sidle; -- sidle is init will be to fetch
   elsif (CLK'event) and (CLK='1') then
      if (EN = '1') then
         pstate <= nstate;
      end if ;
   end if;
end process;

--konecny automat next state
fsm_nstate: process (pstate, i_dec_type, OUT_BUSY, DATA_RDATA, cnt_zero, data_rdata_zero, IN_DATA, IN_VLD)
begin
   --default state
   OUT_WE <= '0';
   IN_REQ <= '0';
   CODE_EN <= '1';
   pc_reg_dec <= '0';
   pc_reg_inc <= '0';
   cnt_reg_dec <= '0';
   cnt_reg_inc <= '0';
   tmp_ld <= '0';
   ptr_reg_dec <= '0';
   ptr_reg_inc <= '0';
   sel <= "00";
   DATA_RDWR <= '0';
   DATA_EN <= '0';

   case pstate is
      --init
      when sidle =>
         CODE_EN <= '1';
         nstate <= decode;

	
      --instr decode
      when decode =>
         case i_dec_type is 
            when inc_ptr =>
                nstate <= s_inc_ptr;
            when dec_ptr =>
               nstate <= s_dec_ptr;
            when inc_value =>
               nstate <= s_inc_value;
            when dec_value =>
               nstate <= s_dec_value;
            when whl_start =>
               nstate <= s_whl_start;
            when whl_end =>
               nstate <= s_whl_end;
            when print =>
               nstate <= s_print;
            when load =>
               nstate <= s_load;
            when tmp_store =>
               nstate <= s_tmp_store;
            when tmp_send =>
               nstate <= s_tmp_send;
            when halt =>
               nstate <= s_halt;
            when others =>
               nstate <= s_nop;
         end case;

      -- >
      when s_inc_ptr =>
         ptr_reg_inc <= '1';
         pc_reg_inc <= '1';
         nstate <= sidle; -- go back to start

      -- <
      when s_dec_ptr =>
         ptr_reg_dec <= '1';
         pc_reg_inc <= '1';
         nstate <= sidle; -- go back to start

      -- +
      when s_inc_value =>
         DATA_EN <= '1'; -- povoleni ram pameti
         DATA_RDWR <= '1'; -- cteni
         nstate <= s_inc_value2; -- posun na dalsi takt

      when s_inc_value2 =>
         DATA_EN <= '1'; -- povoleni ram pameti
         sel <= "11"; -- vybrani data_rdata + 1
         DATA_RDWR <= '0'; -- zapis do pameti
         pc_reg_inc <= '1'; -- inkrementace pc_reg
         nstate <= sidle; -- go back to start

      -- -
      when s_dec_value =>
         DATA_EN <= '1'; -- povoleni ram pameti
         DATA_RDWR <= '1'; -- cteni
         nstate <= s_dec_value2; -- posun na dalsi takt

      when s_dec_value2 =>
         DATA_EN <= '1'; -- povoleni ram pameti
         sel <= "10"; -- mux data -1
         DATA_RDWR <= '0'; -- zapis
         pc_reg_inc <= '1'; 
         nstate <= sidle;

      -- .
      when s_print =>
         DATA_EN <= '1'; -- ram
         DATA_RDWR <= '1'; -- cteni
         nstate <= s_print1;

      when s_print1 =>
         if (OUT_BUSY = '1') then
            nstate <= s_print1; -- pokud je zaneprazdnen cykli tu
         else
            OUT_DATA <= DATA_RDATA; -- prirad vystupni data
            OUT_WE <= '1'; -- povol vystup
            pc_reg_inc <= '1'; -- pc + 1
            nstate <= sidle; -- go back to start
         end if ;
      
      -- ,
      when s_load =>
         IN_REQ <= '1';
         nstate <= s_load2;

      when s_load2 =>
         nstate <= s_load2;
         IN_REQ <= '1';
         if (IN_VLD = '1') then
             nstate <= sidle;
             sel <= "00"; --vstup
             DATA_RDWR <= '0'; -- zapis
             DATA_EN <= '1'; -- ram
             pc_reg_inc <= '1';
          end if ; 

      -- $
      when s_tmp_store =>
         DATA_EN <= '1'; -- povoleni ram pameti
         DATA_RDWR <= '1'; -- cteni
         nstate <= s_tmp_store2; -- posun na dalsi takt

      when s_tmp_store2 =>
         tmp_ld <= '1'; -- ulozeni
         pc_reg_inc <= '1';
         nstate <= sidle;

      -- !
      when s_tmp_send =>
         DATA_EN <= '1'; -- povoleni ram pameti
         sel <= "01"; -- vybrani data_rdata + 1
         DATA_RDWR <= '0'; -- zapis do pameti
         pc_reg_inc <= '1'; -- inkrementace pc_reg
         nstate <= sidle; -- go back to start

      -- [
      when s_whl_start =>
         pc_reg_inc <= '1';
         DATA_RDWR <= '1'; -- cteni
         DATA_EN <= '1'; -- ram
         nstate <= s_whl_start2;

      when s_whl_start2 =>
         nstate <= sidle;
         -- vyhodnoceni data = 0
        if (data_rdata_zero = '1') then
            nstate <= s_whl_start3;
            cnt_reg_inc <= '1';      
         end if ;

      when s_whl_start3 =>
         nstate <= sidle;
         -- cnt != 0
         if (cnt_zero = '0') then
            CODE_EN <= '1';
            nstate <= s_whl_start4;
         end if ;

      when s_whl_start4 =>
         -- if (c==[])..
         nstate <= s_whl_start3;
         if (i_dec_type = whl_start) then
            cnt_reg_inc <= '1';
         elsif (i_dec_type = whl_end) then
            cnt_reg_dec <= '1';
         end if ;
         pc_reg_inc <= '1';

      -- ]
      when s_whl_end =>
         DATA_RDWR <= '1'; --cteni
         DATA_EN <= '1';
         nstate <= s_whl_end2;

      when s_whl_end2 =>
         -- if ram[ptr] = 0
         if (data_rdata_zero = '1') then
            pc_reg_inc <= '1';
            nstate <= sidle; 
         else
            cnt_reg_inc <= '1';
            pc_reg_dec <= '1';
            nstate <= s_whl_end3;
         end if ;

      when s_whl_end3 =>
         nstate <= sidle;
         -- cnt != 0
         if (cnt_zero = '0') then
            CODE_EN <= '1';
            nstate <= s_whl_end4;
         end if ;

      when s_whl_end4 =>
         nstate <= s_whl_end5;
         -- if (c==[])..
         if (i_dec_type = whl_start) then
            cnt_reg_dec <= '1';
         elsif (i_dec_type = whl_end) then
            cnt_reg_inc <= '1';
         end if ;

      when s_whl_end5 =>
         -- if (cnt = 0)..
         nstate <= s_whl_end3;
         if (cnt_zero = '1') then
            pc_reg_inc <= '1';
         else
            pc_reg_dec <= '1';
         end if ;

      -- halt
      when s_halt =>
         nstate <= s_halt;

      -- comment
      when s_nop =>
         pc_reg_inc <= '1';
         nstate <= sidle;

      when others => null;

   end case;
end process;

end behavioral;
