// license:BSD-3-Clause
// copyright-holders:Vas Crabb
/*
 Williams Star Rider

 One of the more sophisticated LaserDisc games.  The "video expander"
 allows part of the LaserDisc backdrop to be enlarged and panned
 horizontally.  Overlay graphics are also supported.

 Major components:
 -----------------
 * Modified Pioneer PR8210 LaserDisc player
 * C-9928 Processor Interface board (PIF)
 * C-9994 throttle opto board
 * C-9995 steering opto board
 * C-9996 brake opto board
 * D-9837 NTSC to RGB board (NTSC)
 * D-9922 CPU board (CPU)
 * D-9924 Video Graphics Generator board (VGG)
 * D-9926 Image ROM board (ROM)
 * D-9930 Video Expander board (EXP)
 * D-9941 Sound System board (Sound)

 C-9928 PIF:
 -----------
 Handles communication with the LaserDisc player.  Uses a 68029E CPU
 with 4KiB of ROM and 8KiB of RAM.

 * CR1 4MHz crystal oscillator
 * J1 4-pin SIL connector
 * J2 15-pin SIL connector (host interface)
   - pin 1 connected to ground
   - pin 2 connected to MPU and PIA reset (active low)
   - pins 3-10 connected to PIA PA0-7
   - pins 11-12 connected to PIA CA1-2
   - pin 13 input is used to generate IRQ interrupts
   - pin 14 is composite sync, connected directly to J3 pin 7
   - pin 15 is even/odd field signal, inverted and connected to PIA PB6
 * J3 26-pin DIL connector
 * U1 6821 PIA
   - IRQA connects to FIRQ
   - IRQB connects to NMI
   - PA/CA are used for host communication
   - PB/CB are used for LaserDisc player control
 * U2 2k*8 RAM
 * U3 2732 4k*8 PROM
 * U4 6809E microprocessor
 * U5 unpopulated (74LS245 - would be used for a different player type)
 * U6 74LS74 dual D-type flip-flop
   - used to generate IRQ interrupts
 * U7 74LS04 hex inverter
 * U8 74LS74 dual D-type flip-flop
   - generates 1MHz 2-phase Q and E clocks for CPU and PIA
 * U9 74LS138 1-of-8 decoder/demultiplexer
   - decodes A15/A14/A13
 * U10 74LS10 triple 3-input NAND gate
 * U11 74LS244 octal tri-state line driver/buffer
   - used to buffer signals to/from LaserDisc player

 D-9837 NTSC:
 ------------
 NTSC to analog RGB and composite sync, based around RCA CA3143E
 luminance processor (U1) and RCA CA3151G chroma processor/demodulator
 (U2).

 D-9922 CPU:
 ----------
 Clocks are generated by the VGG board.  The CPU address and data buses
 are exposed to the VGG board.  There are separate address/data buffers
 for peripherals on the CPU board and VGG board.

 * J2 9-pin SIL connector
 * J3 12-pin SIL connector
 * J4 10-pin SIL connector
 * J5 12-pin SIL connector
 * J6 7-pin SIL connector
 * J7 10-pin SIL connector (power)
   - pin 1-4 connected to ground
   - pin 5 is not present (key)
   - pin 6-9 supply +5V
   - pin 10 supplies +12V unregulated (used by reset circuit)

 W1 (27128) populated
 W2 (2764) unpopulated
 W3 (ROM) unpopulated -  would map U7 at 0xd000-0xdff for 2732 2k*8 ROM
 W4 (RAM) populated - maps U7 at 0xd800-dfff for 6116 2k*8 RAM
 W5 populated - connects /WRX to U7 for 6116 2k*8 RAM
 W6 unpopulated - would connect A11 to U7 for 2732 4k*8 ROM
 W7 (UPRIGHT) populated

 /FXXX+EXXX (active low)   111xxxxx xxxxxxxx      0xe000-0xefff

 U5.6    /R5          x000 100xxxxx xxxxxxxx  0x0:0x8000-0x9fff

 U23.3 (active low)        1x1xxxxx xxxxxxxx      0xa000-0xffff
                           11xxxxxx xxxxxxxx
 U23.11 (active low)       101xxxxx xxxxxxxx      0xa000-0xbfff
                           110xxxxx xxxxxxxx      0xc000-0xdfff

 U24.15  /C0               11000xxx xxxxxxxx      0xc000-0xc7ff
 U24.14  /C8               11001xxx xxxxxxxx      0xc800-0xcfff
 U24.13  /D0               11010xxx xxxxxxxx      0xd000-0xd7ff
 U24.12  /D8               11011xxx xxxxxxxx      0xd800-0xffff
 U24.11  /A0               10100xxx xxxxxxxx      0xa000-0xa7ff
 U24.10  /A8               10101xxx xxxxxxxx      0xa800-0xafff
 U24.9   /B0               10110xxx xxxxxxxx      0xb000-0xb7ff
 U24.7   /B8               10111xxx xxxxxxxx      0xb800-0xbfff

 U19.8 (active low)        110011xx xxxxxxxx      0xcc00-0xcfff
 U19.11 (active low)       1100100x xxxxxxxx      0xc800-0xc9ff

 U19.4                     11001000 0xxxxxxx      0xc800-0xc87f
 U19.5                     11001000 1xxxxxxx      0xc880-0xc8ff
 U19.6                     11001001 0xxxxxxx      0xc900-0xc97f
 U19.7                     11001001 1xxxxxxx      0xc980-0xc9ff

 U28.15  /R1          x000 00xxxxxx xxxxxxxx  0x0:0x0000-0x3fff
 U28.14  /R2          x100 00xxxxxx xxxxxxxx  0x4:0x0000-0x3fff
 U28.13  /R3          x000 01xxxxxx xxxxxxxx  0x0:0x4000-0x7fff
 U28.12  /R4          x100 01xxxxxx xxxxxxxx  0x4:0x4000-0x7fff

 D-9924 VGG:
 -----------

 * U96 2148 Palette blue (low nybble of odd bytes)
 * U97 2148 Palette luminance (high nybble of odd bytes)
 * U98 2148 Palette red (low nybble of even bytes)
 * U99 2148 Palette green (high nybble of even bytes)

 Master clock patterns:
 12MHz  _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
 6MHz   __--__--__--__--__--__--__--__--__--__--__--__--__--__--__--__--__--__--
 E      ____________------------____________------------____________------------
 /RAS   ________----________----________----________----________----________----
        ----______------______------______------______------______------______--
 MUX0   __--------____--------____--------____--------____--------____--------__
 MUX1   __________------------____________------------____________------------__
 Q      ______------------____________------------____________------------______
 LATCH  ______________--------________________--------________________--------__
        __----__----__----__----__----__----__----__----__----__----__----__----
 /SRL   --------____--------------------____--------------------____------------
 4MHz   ___---___---___---___---___---___---___---___---___---___---___---___---
 2MHz   ______------______------______------______------______------______------

 Video RAM consists of 6 4416 16k*4 DRAMs at U25, U26, U41, U42, U55 and
 U56.  The CPU and DMA chips have their addressing mangled by the 6349
 512*8 PROM at U74 to make the visible area appear linear.

 +-----+--------+--------+--------+--------+
 | MUX |   0    |   1    |   2    |   3    |
 +-----+--------+--------+--------+--------+
 | MA7 | VA7    | (1)    | A1     | (1)    |
 | MA6 | VA6    | VA13   | A0     | A7     |
 | MA5 | VA5    | VA12   | PA5    | A6     |
 | MA4 | VA4    | VA11   | PA4    | A5     |
 | MA3 | VA3    | VA10   | PA3    | A4     |
 | MA2 | VA2    | VA9    | PA2    | A3     |
 | MA1 | VA1    | VA8    | PA1    | A2     |
 | MA0 | VA0    | (VA13) | PA0    | (A7)   |
 +-----+--------+--------+--------+--------+

 +-----+-----+-----+-----+-----+
 | PA6 |  0  |  1  |  0  |  1  |
 | PA7 |  0  |  0  |  1  |  1  |
 +-----+-----+-----+-----+-----+
 |  L  | U26 | U42 | U56 |     |
 |  U  | U25 | U41 | U55 |     |
 +-----+-----+-----+-----+-----+

 U2.3    /B/G 9            xxxxx xx01 xxxxxxxx xxxxxxxx
 U2.6    /CS TOP           xxxxx xxxx 11xxxxxx xxxxxxxx
 U2.11 (active low)        xxxxx xx11 xxxxxxxx xxxxxxxx

 U14.6 (active low)        1111x xxxx xxxxxxxx xxxxxxxx

 U15.13 (active high)      xxxxx xx11 xxxxxxxx xxxxxxxx (W)

 U18.8 (active low)        xxxxx xx10 xxxxxxxx xxxxxxxx (CPU access)

 U46.3   /1E               11110 xxxx xxxxxxxx xxxxxxxx
 U46.8   /1F               11111 xxxx xxxxxxxx xxxxxxxx
 U46.11 (active low)       11110 xx01 xxxxxxxx xxxxxxxx

 U33.8 (active low)        11001011 xxxxxxxx      0xcb00-0xcbff

 U35.15  /CB80             11001011 1000xxxx      0xcb80-0xcb8f
 U35.14  /CB90             11001011 1001xxxx      0xcb90-0xcb9f
 U35.13  /CBA0             11001011 1010xxxx      0xcba0-0xcbaf
 U35.12  /DMA              11001011 1011xxxx      0xcbb0-0xcbbf
 U35.11                    11001011 1100xxxx      0xcbc0-0xcbcf (W)
 U35.10  /CBD0             11001011 1101xxxx      0xcbd0-0xcbdf (W)
 U35.9   /CBE0             11001011 1110xxxx      0xcbe0-0xcbef (W)

 U36.1 (active high)       110010xx xxxxxxxx      0xc800-0xcbff

 D-9926 ROM:
 -----------
 Wired for 48 27128 16k*8 PROMs for the main layer and a single 27128
 16k*8 PROM for the high-priority text layer.  The board is laid out to
 allow for a second bank of 48 16k*8 PROMs (possibly intented to be
 selected with A15).  There are provisions for in-circuit programming,
 but the lines are disabled using jumpers during normal operation.

 +------+-----+-----+-----+-----+
 | PP4  |  0  |  0  |  1  |  1  |
 | A15  |  0  |  0  |  0  |  0  |
 | A14  |  0  |  1  |  0  |  1  |
 +------+-----+-----+-----+-----+
 | 0000 | U4  | U19 | U37 | U51 |
 | 0001 | U5  | U20 | U38 | U52 |
 | 0010 | U6  | U21 | U39 | U53 |
 | 0011 | U7  | U22 | U40 | U54 |
 | 0100 | U8  | U23 | U41 | U55 |
 | 0101 | U9  | U24 | U42 | U56 |
 | 0110 | U10 | U25 | U43 | U57 |
 | 0111 | U11 | U26 | U44 | U58 |
 | 1000 | U12 | U27 |     |     |
 | 1001 | U13 | U28 |     |     |
 | 1010 | U14 | U29 |     |     |
 | 1011 | U15 | U30 |     |     |
 | 1100 | U33 | U47 |     |     |
 | 1101 | U34 | U48 |     |     |
 | 1110 | U35 | U49 |     | U46 |
 | 1111 | U36 | U50 |     |     |
 +------+-----+-----+-----+-----+
 * U46 will only be selected using A and PP if B/G 9 is asserted
 * U46 will also be selected if CS TOP is asserted

 D-9941 Sound:
 -------------
 Supports up to 56KiB of ROM (48KiB low and 8KiB high), but only 8KiB is
 populated.  Supports up to 4KiB of RAM, but only 2KiB is populated.
 The main sound generation uses 1408 8-bit R2R DACs driven through 2812
 32*8 FIFOs.  The board is wired for three, but only two are populated.
 The FIFOs can be stepped under CPU control or using the outputs of a
 6840 PTM.  There's also provision for a 5220 speech chip, but it's
 unpopulated.

 There's space for a 64-pin DIL connector exposing the CPU address, data
 and control signals, and providing in-circut ROM programming capability,
 but it isn't populated.


 TODO:
 * Everything
 * Map the banked CPU ROM
 * Video DMA etc.
 * Need a dump of the clock state machine PROM at VGG U114 (82S123 32*8)
 * Need dumps of colour mapping PROMs at VGG U10 and U11 (82S137 1204*4)

 */

#include "emu.h"

#include "cpu/m6809/m6809.h"
#include "machine/2812fifo.h"
#include "machine/6821pia.h"
#include "machine/6840ptm.h"
#include "machine/bankdev.h"
#include "machine/input_merger.h"
#include "machine/nvram.h"

#include "emupal.h"

#include <algorithm>
#include <iterator>
#include <memory>


namespace {

class sr_state : public driver_device
{
public:
	sr_state(machine_config const &mconfig, device_type type, char const *tag)
		: driver_device(mconfig, type, tag)
		, m_main_cpu(*this, "cpu.u46")
		, m_main_pia1(*this, "cpu.u10")
		, m_main_pia2(*this, "cpu.u20")
		, m_main_banks(*this, "cpu.page")
		, m_main_nvram(*this, "cpu.u9")
		, m_main_mem_prot(*this, "MEMPROT")
		, m_main_led(*this, "cpu.u3")
		, m_vgg_pia(*this, "vgg.u7")
		, m_vgg_palette(*this, "vgg.color")
		, m_vgg_horz(*this, "horz")
		, m_pif_cpu(*this, "pif.u4")
		, m_pif_pia(*this, "pif.u1")
		, m_sound_cpu(*this, "sound.u5")
		, m_sound_ptm(*this, "sound.u14")
		, m_sound_pia1(*this, "sound.u34")
		, m_sound_pia2(*this, "sound.u15")
		, m_sound_fifos(*this, { "sound.u23", "sound.u22" })
	{
	}

	void starrider(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(sound_sw1_changed);

protected:
	void driver_start() override;

private:
	u8 cpu_page_r(address_space &space);
	void cpu_page_w(u8 data);
	u8 cpu_led_r(address_space &space);
	void cpu_led_w(u8 data);
	u8 cpu_wd_r(address_space &space);
	void cpu_wd_w(u8 data);
	u8 cpu_nvram_r(address_space &space, offs_t offset);
	void cpu_nvram_w(offs_t offset, u8 data);
	void cpu_pia2_ca2_w(int state);

	static constexpr u16 vgg_drams_map(u16 v, u16 h, u16 p);
	u16 vgg_drams_map(u16 a) const;

	void vgg_drams_w(offs_t offset, u8 data);
	void vgg_impg_w(u8 data);
	void vgg_xlate_w(u8 data);
	void vgg_disable_w(u8 data);
	void vgg_color_palet_w(u8 data);

	u8 pif_irq_enable_r(address_space &space);
	void pif_irq_enable_w(u8 data);
	u8 pif_irq_ack_r(address_space &space) { m_pif_cpu->set_input_line(M6809_IRQ_LINE, CLEAR_LINE); return space.unmap(); }
	void pif_irq_ack_w(u8 data) { m_pif_cpu->set_input_line(M6809_IRQ_LINE, CLEAR_LINE); }

	template <unsigned N> u8 sound_fifo_pl_r(address_space &space);
	template <unsigned N> void sound_fifo_pl_w(u8 data);
	template <unsigned N> u8 sound_fifo_pd_r(address_space &space);
	template <unsigned N> void sound_fifo_pd_w(u8 data);
	u8 sound_ef1_r(address_space &space);
	void sound_ef1_w(u8 data);
	u8 sound_er1_r(address_space &space);
	void sound_er1_w(u8 data);
	u8 sound_ef2_r(address_space &space);
	void sound_ef2_w(u8 data);
	u8 sound_er2_r(address_space &space);
	void sound_er2_w(u8 data);

	void sound_pia2_pa(u8 data);
	void sound_fifo0_ir(int state);
	void sound_fifo1_ir(int state);
	template <unsigned N> void sound_fifo_or(int state);
	template <unsigned N> void sound_fifo_flag(int state);

	void main_memory(address_map &map);
	void main_banks(address_map &map);
	void pif_memory(address_map &map);
	void sound_memory(address_map &map);

	required_device<cpu_device> m_main_cpu;
	required_device<pia6821_device> m_main_pia1;
	required_device<pia6821_device> m_main_pia2;
	required_device<address_map_bank_device> m_main_banks;
	required_shared_ptr<u8> m_main_nvram;
	required_ioport m_main_mem_prot;
	output_finder<> m_main_led;

	required_device<pia6821_device> m_vgg_pia;
	required_device<palette_device> m_vgg_palette;
	required_region_ptr<u8> m_vgg_horz;

	required_device<cpu_device> m_pif_cpu;
	required_device<pia6821_device> m_pif_pia;

	required_device<cpu_device> m_sound_cpu;
	required_device<ptm6840_device> m_sound_ptm;
	required_device<pia6821_device> m_sound_pia1;
	required_device<pia6821_device> m_sound_pia2;
	required_device_array<fifo2812_device, 2> m_sound_fifos;

	u8 m_main_page = 0U;

	std::unique_ptr<u8 []> m_vgg_drams;
	u8 m_vgg_image_page = 0U;
	u8 m_vgg_xlate = 0U;
	u8 m_vgg_expand_disable = 0U;
	u8 m_vgg_background_disable = 0U;
	u8 m_vgg_color_palet = 0U;

	u8 m_pif_irq_enable = 0U;

	u8 m_sound_ptm_o[2] = { 0U, 0U };
	u8 m_sound_pia2_pa_out = 0xffU;
	u8 m_sound_pia2_pb_in = 0xffU;
};


void sr_state::starrider(machine_config &config)
{
	// CPU
	MC6809E(config, m_main_cpu, 24_MHz_XTAL / 2 / 12); // 6809E - clock patterns generated using 82S123 and 74F374 at VGG U114 and U101
	m_main_cpu->set_addrmap(AS_PROGRAM, &sr_state::main_memory);

	PIA6821(config, m_main_pia1, 24_MHz_XTAL / 2 / 12); // 6820/21
	m_main_pia1->readpa_handler().set_ioport("IN2");
	// CA1 is the /END SCREEN signal
	// CA2 is the 4MS signal
	m_main_pia1->writepb_handler().set(m_sound_pia1, FUNC(pia6821_device::portb_w));
	m_main_pia1->cb2_handler().set(m_sound_pia1, FUNC(pia6821_device::ca1_w));
	m_main_pia1->irqa_handler().set("main.irq", FUNC(input_merger_device::in_w<0>));
	m_main_pia1->irqb_handler().set("main.irq", FUNC(input_merger_device::in_w<1>));

	PIA6821(config, m_main_pia2, 24_MHz_XTAL / 2 / 12); // 6820/21
	m_main_pia2->readpa_handler().set_ioport("IN0");
	m_main_pia2->ca1_w(0); // grounded
	m_main_pia2->ca2_handler().set(FUNC(sr_state::cpu_pia2_ca2_w));
	m_main_pia2->readpb_handler().set_ioport("IN1");
	m_main_pia2->cb1_w(0); // grounded
	// CB2 is unused
	// IRQA and IRQB are unused

	NVRAM(config, "cpu.u9", nvram_device::DEFAULT_ALL_0);

	ADDRESS_MAP_BANK(config, m_main_banks);
	m_main_banks->set_map(&sr_state::main_banks);
	m_main_banks->set_data_width(8);
	m_main_banks->set_addr_width(20);
	m_main_banks->set_stride(0x1'0000);

	INPUT_MERGER_ANY_HIGH(config, "main.irq").output_handler().set_inputline(m_main_cpu, M6809_IRQ_LINE);

	// VGG
	PIA6821(config, m_vgg_pia, 24_MHz_XTAL / 2 / 12); // 6821
	// PA0-8 and CA2 are outputs to the expander board
	// CA1 is the /HALT signal
	// PB0-7 and CB1-2 are the interface to the PIF board (bidirectional)
	m_vgg_pia->irqa_handler().set_inputline(m_main_cpu, M6809_FIRQ_LINE);
	m_vgg_pia->irqb_handler().set("main.irq", FUNC(input_merger_device::in_w<2>));

	PALETTE(config, m_vgg_palette).set_format(palette_device::IBGR_4444, 1024).set_endianness(ENDIANNESS_LITTLE);

	// PIF
	MC6809E(config, m_pif_cpu, 4_MHz_XTAL / 4); // 6809E
	m_pif_cpu->set_addrmap(AS_PROGRAM, &sr_state::pif_memory);

	PIA6821(config, m_pif_pia, 4_MHz_XTAL / 4); // 6821
	m_pif_pia->irqa_handler().set_inputline(m_pif_cpu, M6809_FIRQ_LINE);
	m_pif_pia->irqb_handler().set_inputline(m_pif_cpu, INPUT_LINE_NMI);

	// Sound
	MC6809E(config, m_sound_cpu, 8_MHz_XTAL / 4); // 68B09E
	m_sound_cpu->set_addrmap(AS_PROGRAM, &sr_state::sound_memory);

	PTM6840(config, m_sound_ptm, 8_MHz_XTAL / 4); // 68B40
	m_sound_ptm->o1_callback().set(m_sound_ptm, FUNC(ptm6840_device::set_c1));
	m_sound_ptm->o1_callback().append([this] (int state) { m_sound_fifos[0]->pd_w(m_sound_ptm_o[0] = state); });
	m_sound_ptm->o2_callback().set(m_sound_ptm, FUNC(ptm6840_device::set_c2));
	m_sound_ptm->o2_callback().append([this] (int state) { m_sound_fifos[1]->pd_w(m_sound_ptm_o[1] = state); });
	m_sound_ptm->o3_callback().set(m_sound_ptm, FUNC(ptm6840_device::set_c3));
	m_sound_ptm->irq_callback().set("sound.irq", FUNC(input_merger_device::in_w<0>));

	PIA6821(config, m_sound_pia1, 8_MHz_XTAL / 4); // 68B21
	m_sound_pia1->ca2_handler().set(m_main_pia1, FUNC(pia6821_device::cb1_w));
	// PB0-7 are for unpopulated 5220 at U55
	// CB1-2 are unused
	m_sound_pia1->irqa_handler().set_inputline(m_sound_cpu, M6809_FIRQ_LINE);
	m_sound_pia1->irqb_handler().set("sound.irq", FUNC(input_merger_device::in_w<1>));

	PIA6821(config, m_sound_pia2, 8_MHz_XTAL / 4); // 68B21
	m_sound_pia2->writepa_handler().set(FUNC(sr_state::sound_pia2_pa));
	// PA3 is the IR output from the unpopulated third FIFO (active low)
	// PA7 is the RDY output from the unpopulated 5220 (active low)
	// CB1-2 are unused
	m_sound_pia1->irqb_handler().set("sound.irq", FUNC(input_merger_device::in_w<2>));
	m_sound_pia1->irqb_handler().set("sound.irq", FUNC(input_merger_device::in_w<3>));

	FIFO2812(config, m_sound_fifos[0]);
	m_sound_fifos[0]->ir_cb().set(FUNC(sr_state::sound_fifo0_ir));
	//m_sound_fifos[0]->ir_cb().append(...) IR is input to a clock-stretching circuit
	m_sound_fifos[0]->or_cb().set(FUNC(sr_state::sound_fifo_or<0>));
	m_sound_fifos[0]->flag_cb().set(FUNC(sr_state::sound_fifo_flag<0>));

	FIFO2812(config, m_sound_fifos[1]);
	m_sound_fifos[1]->ir_cb().set(FUNC(sr_state::sound_fifo1_ir));
	//m_sound_fifos[1]->ir_cb().append(...) IR is input to a clock-stretching circuit
	m_sound_fifos[1]->or_cb().set(FUNC(sr_state::sound_fifo_or<1>));
	m_sound_fifos[1]->flag_cb().set(FUNC(sr_state::sound_fifo_flag<1>));

	INPUT_MERGER_ANY_HIGH(config, "sound.irq").output_handler().set_inputline(m_sound_cpu, M6809_IRQ_LINE);
}


INPUT_CHANGED_MEMBER(sr_state::sound_sw1_changed)
{
	// TODO: 4.7kΩ/0.01µF RC time constant to stretch/debounce this
	m_sound_cpu->set_input_line(INPUT_LINE_NMI, newval ? ASSERT_LINE : CLEAR_LINE);
}


void sr_state::driver_start()
{
	m_main_led.resolve();
	m_vgg_drams = std::make_unique<u8 []>(16'384 * 4 / 8 * 6);

	save_item(NAME(m_main_page));
	save_pointer(NAME(m_vgg_drams), 16'384 * 4 / 8 * 6);
	save_item(NAME(m_vgg_image_page));
	save_item(NAME(m_vgg_xlate));
	save_item(NAME(m_vgg_expand_disable));
	save_item(NAME(m_vgg_background_disable));
	save_item(NAME(m_vgg_color_palet));
	save_item(NAME(m_pif_irq_enable));
	save_item(NAME(m_sound_ptm_o));
	save_item(NAME(m_sound_pia2_pa_out));
	save_item(NAME(m_sound_pia2_pb_in));

	m_main_page = 0U;
	std::fill_n(m_vgg_drams.get(), 16'384 * 4 / 8 * 6, 0U);
	m_vgg_image_page = 0U;
	m_vgg_xlate = 0U;
	m_vgg_expand_disable = 0U;
	m_vgg_background_disable = 0U;
	m_vgg_color_palet = 0U;
	m_pif_irq_enable = 0U;
	std::fill(std::begin(m_sound_ptm_o), std::end(m_sound_ptm_o), 0U);
	m_sound_pia2_pa_out = 0xffU;
	m_sound_pia2_pb_in = 0xffU;

	for (unsigned i = 0U; 3U > i; ++i)
	{
		m_sound_ptm->set_gate(i, 0);
		m_sound_ptm->set_clock(i, 0);
	}
}


u8 sr_state::cpu_page_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		cpu_page_w(data);
	return data;
}

void sr_state::cpu_page_w(u8 data)
{
	// 4 bits latched by U43 (74LS173)
	m_main_page = data & 0x0f;
	m_main_banks->set_bank(m_main_page);
}

u8 sr_state::cpu_led_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		cpu_led_w(data);
	return data;
}

void sr_state::cpu_led_w(u8 data)
{
	// latched by U16 (74LS374)
	m_main_led = bitswap<8>(~data, 0, 1, 2, 3, 4, 5, 6, 7);
}

u8 sr_state::cpu_wd_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		cpu_wd_w(data);
	return data;
}

void sr_state::cpu_wd_w(u8 data)
{
	// U22 (74LS161) parallel load zero
	if ((data & 0x3e) == 0x14)
	{
		/* TODO: watchdog reset */
	}
}

u8 sr_state::cpu_nvram_r(address_space &space, offs_t offset)
{
	// U9 (5514) is 4 bits wide
	return (m_main_nvram[offset] & 0x0f) | (space.unmap() & 0xf0);
}

void sr_state::cpu_nvram_w(offs_t offset, u8 data)
{
	// memory protect switch prevents writes to the first quarter of NVRAM
	if ((offset & 0x300) || BIT(m_main_mem_prot->read(), 0))
		m_main_nvram[offset] = data & 0x0f;
}

void sr_state::cpu_pia2_ca2_w(int state)
{
	m_sound_cpu->set_input_line(INPUT_LINE_RESET, state ? ASSERT_LINE : CLEAR_LINE);
	for (auto const &fifo : m_sound_fifos)
		fifo->mr_w(state ? 0 : 1);

	// TODO: these devices don't have reset line inputs exposed
	if (state)
	{
		m_sound_ptm->reset();
		m_sound_pia1->reset();
		m_sound_pia2->reset();
	}
}


constexpr u16 sr_state::vgg_drams_map(u16 v, u16 h, u16 p)
{
	// addressing arranged so video display meets refresh requirements
	// pixels are striped across U25/U26/U41/U42/U55/U56
	// 8-bit DRAM row is 6-bit horizontal counter and two lower bits of 8-bit vertical counter
	// 6-bit DRAM column is upper six bits of 8-bit vertical counter
	return ((p << 14) & 0xc000) | ((v << 12) & 0x3000) | ((h << 6) & 0x0fc0) | ((v >> 2) & 0x003f);
}

u16 sr_state::vgg_drams_map(u16 a) const
{
	// HORZ PROM U74 (6349) is used to map a 320*256 area at linear column-major addresses
	// A5 is tied low (only even 32-byte chunks of the 512*8 BPROM are used)
	u16 const pa(m_vgg_horz[((a >> 7) & 0x01c0) | ((a >> 8) & 0x001f)]);
	return vgg_drams_map(a & 0x00ff, pa & 0x003f, (pa >> 6) & 0x0003);
}


void sr_state::vgg_drams_w(offs_t offset, u8 data)
{
	if (((m_main_page & 0x03) == 0x03) || (((m_main_page & 0x03) == 0x01) && ((m_vgg_image_page & 0x1f) == 0x1e)))
		offset ^= 0x8000;
	u16 const addr(vgg_drams_map(offset));
	if ((offset & 0xc000) != 0xc000)
		m_vgg_drams[addr] = data;
}

void sr_state::vgg_impg_w(u8 data)
{
	// 6 bits latched by U13 (74LS173) but only 5 bits used
	m_vgg_image_page = data & 0x3f;
}

void sr_state::vgg_xlate_w(u8 data)
{
	// 6 bits latched by U12 (74LS173)
	m_vgg_xlate = data & 0x3f;
}

void sr_state::vgg_disable_w(u8 data)
{
	// latched by U111 (74LS74)
	m_vgg_background_disable = BIT(data, 0); // actual signal is active low
	m_vgg_expand_disable = BIT(data, 1);
}

void sr_state::vgg_color_palet_w(u8 data)
{
	// all 8 bits latched by U94 (74LS374) but only 6 bits used
	m_vgg_color_palet = data;
}


u8 sr_state::pif_irq_enable_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		pif_irq_enable_w(data);
	return data;
}

void sr_state::pif_irq_enable_w(u8 data)
{
	m_pif_irq_enable = BIT(data, 0);
}


template <unsigned N> u8 sr_state::sound_fifo_pl_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		sound_fifo_pl_w<N>(data);
	return data;
}

template <unsigned N> void sr_state::sound_fifo_pl_w(u8 data)
{
	// TODO: input to a clock-stretching circuit
	m_sound_fifos[N]->write(data);
}

template <unsigned N> u8 sr_state::sound_fifo_pd_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		sound_fifo_pd_w<N>(data);
	return data;
}

template <unsigned N> void sr_state::sound_fifo_pd_w(u8 data)
{
	m_sound_fifos[N]->pd_w(m_sound_ptm_o[N] ? 0 : 1);
	m_sound_fifos[N]->pd_w(m_sound_ptm_o[N] ? 1 : 0);
}

u8 sr_state::sound_ef1_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		sound_ef1_w(data);
	return data;
}

void sr_state::sound_ef1_w(u8 data)
{
	data &= 0x3f; // DAC 1 -> front mix level (6 bits latched by U19)
}

u8 sr_state::sound_er1_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		sound_er1_w(data);
	return data;
}

void sr_state::sound_er1_w(u8 data)
{
	data &= 0x3f; // DAC 1 -> rear mix level (6 bits latched by U2)
}

u8 sr_state::sound_ef2_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		sound_ef2_w(data);
	return data;
}

void sr_state::sound_ef2_w(u8 data)
{
	data &= 0x3f; // DAC 2 -> front mix level (6 bits latched by U17)
}

u8 sr_state::sound_er2_r(address_space &space)
{
	u8 const data(space.unmap());
	if (!machine().side_effects_disabled())
		sound_er2_w(data);
	return data;
}

void sr_state::sound_er2_w(u8 data)
{
	data &= 0x3f; // DAC 2 -> rear mix level (6 bits latched by U36)
}


void sr_state::sound_pia2_pa(u8 data)
{
	u8 const diff(data ^ m_sound_pia2_pa_out);
	m_sound_pia2_pa_out = data;
	if (diff & ~m_sound_pia2_pb_in & 0x77U)
	{
		m_sound_pia2->portb_w(m_sound_pia2_pb_in | (m_sound_pia2_pa_out & 0x77U));
		m_sound_pia2->ca1_w((0x07U == ((m_sound_pia2_pb_in | m_sound_pia2_pa_out) & 0x07U)) ? 1 : 0);
		m_sound_pia2->ca2_w((0x70U == ((m_sound_pia2_pb_in | m_sound_pia2_pa_out) & 0x70U)) ? 1 : 0);
	}
}

void sr_state::sound_fifo0_ir(int state)
{
	if (state)
		m_sound_pia2_pb_in |= 0x08U;
	else
		m_sound_pia2_pb_in &= 0xf7U;
	m_sound_pia2->portb_w(m_sound_pia2_pb_in | (m_sound_pia2_pa_out & 0x77U));
}

void sr_state::sound_fifo1_ir(int state)
{
	if (state)
		m_sound_pia2_pb_in |= 0x80U;
	else
		m_sound_pia2_pb_in &= 0x7fU;
	m_sound_pia2->portb_w(m_sound_pia2_pb_in | (m_sound_pia2_pa_out & 0x77U));
}

template <unsigned N> void sr_state::sound_fifo_or(int state)
{
	if (state)
		m_sound_pia2_pb_in |= u8(1) << (N + 4);
	else
		m_sound_pia2_pb_in &= ~(u8(1) << (N + 4));
	if (!BIT(m_sound_pia2_pa_out, N + 4))
	{
		m_sound_pia2->portb_w(m_sound_pia2_pb_in | (m_sound_pia2_pa_out & 0x77U));
		m_sound_pia2->ca2_w((0x70U == ((m_sound_pia2_pb_in | m_sound_pia2_pa_out) & 0x70U)) ? 1 : 0);
	}
}

template <unsigned N> void sr_state::sound_fifo_flag(int state)
{
	if (state)
		m_sound_pia2_pb_in |= u8(1) << N;
	else
		m_sound_pia2_pb_in &= ~(u8(1) << N);
	if (!BIT(m_sound_pia2_pa_out, N))
	{
		m_sound_pia2->portb_w(m_sound_pia2_pb_in | (m_sound_pia2_pa_out & 0x77U));
		m_sound_pia2->ca1_w((0x07U == ((m_sound_pia2_pb_in | m_sound_pia2_pa_out) & 0x07U)) ? 1 : 0);
	}
}


void sr_state::main_memory(address_map &map)
{
	map(0x0000, 0x9fff).rw(m_main_banks, FUNC(address_map_bank_device::read8), FUNC(address_map_bank_device::write8));
	map(0xa000, 0xbfff).ram(); // U51/U44/U36/U25 (6116)
	map(0xc000, 0xc7ff).ram().w(m_vgg_palette, FUNC(palette_device::write8)).share("vgg.color"); // U96/U97/U98/U99 (2148)

	map(0xc800, 0xc800).mirror(0x007f).rw(FUNC(sr_state::cpu_page_r), FUNC(sr_state::cpu_page_w));
	map(0xc880, 0xc880).mirror(0x007f).rw(FUNC(sr_state::cpu_led_r), FUNC(sr_state::cpu_led_w));
	map(0xc900, 0xc900).mirror(0x007f).rw(FUNC(sr_state::cpu_wd_r), FUNC(sr_state::cpu_wd_w));
	map(0xc980, 0xc983).mirror(0x0070).rw(m_main_pia1, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0xc984, 0xc987).mirror(0x0070).rw(m_main_pia2, FUNC(pia6821_device::read), FUNC(pia6821_device::write));

	map(0xcb80, 0xcb83).mirror(0x000c).rw(m_vgg_pia, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	//map(0xcb90, 0xcb90).mirror(0x000f).r(FUNC(sr_state::...)); inverted FIELD signal on D7
	//map(0xcba0, 0xcba0).mirror(0x000f).r(FUNC(sr_state::...)); vertical counters (video address A13..A6)
	//map(0xcbb0, 0xcbbf) DMA
	map(0xcbc0, 0xcbc0).mirror(0x000e).w(FUNC(sr_state::vgg_impg_w));
	map(0xcbc1, 0xcbc1).mirror(0x000e).w(FUNC(sr_state::vgg_xlate_w));
	map(0xcbd0, 0xcbd0).mirror(0x000f).w(FUNC(sr_state::vgg_disable_w));
	map(0xcbe0, 0xcbe0).mirror(0x000f).w(FUNC(sr_state::vgg_color_palet_w));

	map(0xcc00, 0xcfff).rw(FUNC(sr_state::cpu_nvram_r), FUNC(sr_state::cpu_nvram_w)).share(m_main_nvram);
	map(0xd000, 0xd7ff).ram(); // U14 (6116)
	//map(0xd800, 0xdfff).ram() U7 (6116) unpopulated
	map(0xe000, 0xffff).rom().region("fixed", 0x0000);
}

void sr_state::main_banks(address_map &map)
{
	// page 0x0/0x8 read
	map(0x0'0000, 0x0'3fff).mirror(0x8'0000).rom().region("banked", 0x0'0000); // /R1 -> U8
	map(0x0'4000, 0x0'7fff).mirror(0x8'0000).rom().region("banked", 0x0'8000); // /R3 -> U26
	map(0x0'8000, 0x0'9fff).mirror(0x8'0000).rom().region("banked", 0x1'0000); // /R5 -> U45

	// page 0x4/0xc read
	map(0x4'0000, 0x4'3fff).mirror(0x8'0000).rom().region("banked", 0x0'4000); // /R2 -> U15
	map(0x4'4000, 0x4'7fff).mirror(0x8'0000).rom().region("banked", 0x0'c000); // /R4 -> U37
	// nothing in 0x4'8000-0x4'9fff

	// page 0x0-0x7 write
	map(0x0'0000, 0x0'9fff).mirror(0x7'0000).w(FUNC(sr_state::vgg_drams_w));
}

void sr_state::pif_memory(address_map &map)
{
	map(0x0000, 0x1fff).ram();
	map(0x2000, 0x2000).mirror(0x1fff).rw(FUNC(sr_state::pif_irq_enable_r), FUNC(sr_state::pif_irq_enable_w));
	map(0x4000, 0x4000).mirror(0x1fff).rw(FUNC(sr_state::pif_irq_ack_r), FUNC(sr_state::pif_irq_ack_w));
	//map(0x6000, 0x6000).mirror(0x1fff) LD player via U5 if it was populated
	map(0x8000, 0x8003).mirror(0x1ffc).rw(m_pif_pia, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	// nothing in 0xa000-0xdfff
	map(0xe000, 0xefff).mirror(0x1000).rom().region("pif", 0x0000);
}

void sr_state::sound_memory(address_map &map)
{
	map(0x0000, 0xbfff).rom().region("snd", 0x0000);
	map(0xc000, 0xc007).mirror(0x0078).rw(m_sound_ptm, FUNC(ptm6840_device::read), FUNC(ptm6840_device::write));
	map(0xc080, 0xc083).mirror(0x007c).rw(m_sound_pia1, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	//map(0xc100, 0xc17f) U55 (5220) unpopulated
	// nothing in 0xc180-0xc1ff
	map(0xc200, 0xc200).mirror(0x007f).rw(FUNC(sr_state::sound_fifo_pd_r<0>), FUNC(sr_state::sound_fifo_pd_w<0>));
	map(0xc280, 0xc280).mirror(0x007f).rw(FUNC(sr_state::sound_fifo_pd_r<1>), FUNC(sr_state::sound_fifo_pd_w<1>));
	//map(0xc300, 0xc37f) U21 (2821) unpopulated
	map(0xc380, 0xc380).mirror(0x007f).rw(FUNC(sr_state::sound_ef1_r), FUNC(sr_state::sound_ef1_w));
	map(0xc400, 0xc400).mirror(0x007f).rw(FUNC(sr_state::sound_er1_r), FUNC(sr_state::sound_er1_w));
	map(0xc480, 0xc480).mirror(0x007f).rw(FUNC(sr_state::sound_ef2_r), FUNC(sr_state::sound_ef2_w));
	map(0xc500, 0xc500).mirror(0x007f).rw(FUNC(sr_state::sound_er2_r), FUNC(sr_state::sound_er2_w));
	map(0xc580, 0xc583).mirror(0x007c).rw(m_sound_pia2, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	// nothing in 0xc600-0xc67f
	//map(0xc680, 0xc6ff) U21 (2812) unpopulated
	map(0xc700, 0xc700).mirror(0x007f).rw(FUNC(sr_state::sound_fifo_pl_r<1>), FUNC(sr_state::sound_fifo_pl_w<1>));
	map(0xc780, 0xc780).mirror(0x007f).rw(FUNC(sr_state::sound_fifo_pl_r<0>), FUNC(sr_state::sound_fifo_pl_w<0>));
	// nothing in 0xc800-0xcfff
	map(0xd000, 0xd7ff).ram(); // U13 (6116)
	//map(0xd800, 0xdfff) U12 (6116) unpopulated
	map(0xe000, 0xffff).rom().region("snd", 0xe000);
}


INPUT_PORTS_START(starrider)
	PORT_START("IN0")
	PORT_BIT(0x3f, IP_ACTIVE_LOW, IPT_UNKNOWN) // TODO: steering Gray code
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_START1)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_START2)

	PORT_START("IN1")
	PORT_BIT(0x0f, IP_ACTIVE_LOW,  IPT_UNKNOWN) // TODO: accelerator
	PORT_BIT(0x30, IP_ACTIVE_LOW,  IPT_UNKNOWN) // TODO: brake
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_BUTTON1)                  PORT_NAME("Turbo")
	PORT_BIT(0x80, IP_ACTIVE_LOW,  IPT_BUTTON2)      PORT_TOGGLE PORT_NAME("Hi/Lo")

	PORT_START("IN2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_SERVICE1)     PORT_TOGGLE PORT_NAME("Auto Up / Manual Down")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_SERVICE)                  PORT_NAME("Advance")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_MEMORY_RESET)             PORT_NAME("High Score Reset")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_COIN1)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_COIN2)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_COIN3)
	PORT_BIT(0x40, IP_ACTIVE_LOW,  IPT_TILT)
	PORT_CONFNAME(0x80, 0x00, DEF_STR(Upright)) PORT_DIPLOCATION("CPU W7:1")
	PORT_CONFSETTING(0x80, DEF_STR(Off))
	PORT_CONFSETTING(0x00, DEF_STR(On))

	PORT_START("MEMPROT")
	PORT_BIT(0x01, IP_ACTIVE_LOW,  IPT_SERVICE2)     PORT_TOGGLE PORT_NAME("Memory Protect")

	PORT_START("SW1")
	PORT_BIT(0x01, IP_ACTIVE_LOW,  IPT_SERVICE3)                 PORT_NAME("Sound Test")
													 PORT_CHANGED_MEMBER(DEVICE_SELF, sr_state, sound_sw1_changed, 0)
INPUT_PORTS_END


ROM_START(starridr)
	// TODO: ROM labels are illegible in photos
	ROM_REGION(0x2000, "fixed", 0)
	ROM_LOAD("rom_35.u52", 0x0000, 0x2000, CRC(9c9a50bb) SHA1(99f6cfff19c41251645259f5268af6d29d28e48b))

	ROM_REGION(0x1'2000, "banked", 0)
	ROM_LOAD("rom_30.u8",  0x0'0000, 0x4000, CRC(934d98c0) SHA1(8e53b0b024623d88bbaf7a3061925b5d8de37fb3))
	ROM_LOAD("rom_31.u15", 0x0'4000, 0x4000, CRC(69dcb5af) SHA1(a3a6cefa1b8029ba1ca2ab14898f2949a7e13a4b))
	ROM_LOAD("rom_32.u26", 0x0'8000, 0x4000, CRC(b0ed1ed4) SHA1(32f2f06c3359079628e8ec83c8bdb85e170031ea))
	// 0xc000 U37 unpopulated
	ROM_LOAD("rom_34.u45", 0x1'0000, 0x2000, CRC(5b0b9a26) SHA1(3e2f8efa805a797546f984c6538fd353f3f7470b))

	ROM_REGION(0x1000, "pif", 0)
	ROM_LOAD("rom_26.u3", 0x0000, 0x1000, CRC(04e9d9fc) SHA1(c5531f6acf929bc243b5313bee6627d93dac90b9) BAD_DUMP) // reset vector points to RAM - can't be right

	ROM_REGION(0x1'0000, "snd", 0)
	// 0x0000 U8 unpopulated
	// 0x4000 U9 unpopulated
	// 0x8000 U10 unpopulated
	// 0xc000 space for RAM and I/O
	ROM_LOAD("rom_27.u11", 0xe000, 0x2000, CRC(ffacb56d) SHA1(e6f7984e014a5ace50b0b1428a386ae645b972f5))

	ROM_REGION(0x10'0000, "gfx", 0)
	ROM_LOAD("rom_1.u4",   0x0'0000, 0x4000, CRC(9655db83) SHA1(50406ea8a4e20dbc7d4ff3ca60d41d142e3d593e))
	ROM_LOAD("rom_2.u19",  0x0'4000, 0x4000, CRC(8903afe2) SHA1(dce09ab66ad96422e426b8f0eb7bedaad1596be2))
	ROM_LOAD("rom_3.u5",   0x0'8000, 0x4000, CRC(ac1bdd83) SHA1(a635714d814fea7728e3d62003069873d00536b2))
	ROM_LOAD("rom_4.u20",  0x0'c000, 0x4000, CRC(3bef2dc3) SHA1(60e8c794d706ea6ef7b4c25f11fd9850f992bafd))
	ROM_LOAD("rom_5.u6",   0x1'0000, 0x4000, CRC(18470b9c) SHA1(5cf7bbf8c1dfeff9ee570b86d7d02bf7126ad01f))
	ROM_LOAD("rom_6.u21",  0x1'4000, 0x4000, CRC(3d345e8b) SHA1(77a1faa1b2e48bc51b20b3015007d399f72c5fc9))
	ROM_LOAD("rom_7.u7",   0x1'8000, 0x4000, CRC(c2f624db) SHA1(f57c6c16a7cf5a03cb1bee7fba841d70fe6a25d9))
	ROM_LOAD("rom_8.u22",  0x1'c000, 0x4000, CRC(089cc4dd) SHA1(ef15cf517da6ffae9e1063f3fda854ad2ef16e66))
	ROM_LOAD("rom_9.u8",   0x2'0000, 0x4000, CRC(360938be) SHA1(632194a3d398a7f670bfb05f58c1afd8c17ec288))
	ROM_LOAD("rom_10.u23", 0x2'4000, 0x4000, CRC(2fc10697) SHA1(205476c13348140a92254d0998d740f18bff7187))
	ROM_LOAD("rom_11.u9",  0x2'8000, 0x4000, CRC(665a4b07) SHA1(3c57839f65070e2b63b33eaa833370203993dc51))
	ROM_LOAD("rom_12.u24", 0x2'c000, 0x4000, CRC(e117c9b3) SHA1(0500e6723b973ab672f4479bf1ce45579ef9a9f2))
	ROM_LOAD("rom_13.u10", 0x3'0000, 0x4000, CRC(3e8cdea4) SHA1(554ddd87d5a5de1b04b81ca6a730682d90f334a1))
	ROM_LOAD("rom_14.u25", 0x3'4000, 0x4000, CRC(d8260022) SHA1(6eb60d512f6bee3a74950145ab52a6c8bb7054c4))
	ROM_LOAD("rom_15.u11", 0x3'8000, 0x4000, CRC(bac9b2b4) SHA1(0700debb88f7e001c78407f69d804c4340d602e2))
	ROM_LOAD("rom_16.u26", 0x3'c000, 0x4000, CRC(111e2f28) SHA1(96904f08d66fed87fcfc0c59be50546f42e27687))
	ROM_LOAD("rom_17.u12", 0x4'0000, 0x4000, CRC(eb5f96dd) SHA1(2ccd3b1ea12e201306707d8ed95c000c06766b48))
	ROM_LOAD("rom_18.u27", 0x4'4000, 0x4000, CRC(b87eeeee) SHA1(37d6c360c7557d28bb7cc109ac6617569d7afb18))
	ROM_LOAD("rom_19.u13", 0x4'8000, 0x4000, CRC(67ffe21b) SHA1(ebfc8f963e852cf5adb14f7f76f60fd738ddbc18))
	// 0x4'c000 U28 unpopulated
	// 0x5'0000 U14 unpopulated
	// 0x5'4000 U29 unpopulated
	// 0x5'8000 U15 unpopulated
	// 0x5'c000 U30 unpopulated
	// 0x6'0000 U33 unpopulated
	// 0x6'4000 U47 unpopulated
	// 0x6'8000 U34 unpopulated
	// 0x6'c000 U48 unpopulated
	// 0x7'0000 U35 unpopulated
	// 0x7'4000 U49 unpopulated
	// 0x7'8000 U36 unpopulated
	// 0x7'c000 U50 unpopulated
	// 0x8'0000 U37 unpopulated
	// 0x8'4000 U51 unpopulated
	// 0x8'8000 U38 unpopulated
	// 0x8'c000 U52 unpopulated
	// 0x9'0000 U39 unpopulated
	// 0x9'4000 U53 unpopulated
	// 0x9'8000 U40 unpopulated
	// 0x9'c000 U54 unpopulated
	// 0xa'0000 U41 unpopulated
	// 0xa'4000 U55 unpopulated
	// 0xa'8000 U42 unpopulated
	// 0xa'c000 U56 unpopulated
	// 0xb'0000 U43 unpopulated
	// 0xb'4000 U57 unpopulated
	// 0xb'8000 U44 unpopulated
	// 0xb'c000 U58 unpopulated
	ROM_LOAD("rom_25.u46", 0xe'4000, 0x2000, CRC(2812097e) SHA1(793ed5ec8b1d1689e856fc2008f46faf963f5045))
	ROM_RELOAD(0xe'6000, 0x2000)

	ROM_REGION(0x4000, "text", 0)
	ROM_LOAD("rom_25.u46", 0x0000, 0x2000, CRC(2812097e) SHA1(793ed5ec8b1d1689e856fc2008f46faf963f5045))
	ROM_RELOAD(0x2000, 0x2000)

	ROM_REGION(0x20, "clkgen", 0)
	ROM_LOAD("u114.82s123", 0x00, 0x20, CRC(27a6d555) SHA1(988d55092d7d0243a867986873dfd12be67280c7))

	ROM_REGION(0x400, "color", 0)
	ROMX_LOAD("u10.82s137", 0x000, 0x400, CRC(917e35ca) SHA1(676e2d211c81dbbf911f99a672eac1fd29553a2b), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO)
	ROMX_LOAD("u11.82s137", 0x000, 0x400, CRC(917e35ca) SHA1(676e2d211c81dbbf911f99a672eac1fd29553a2b), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI)

	ROM_REGION(0x200, "horz", 0)
	ROM_LOAD("u74.6349", 0x000, 0x200, CRC(362ec0f9) SHA1(0304a36d038436e9f5e817dfc2c40b6421953cad))

	DISK_REGION( "ld_pr8210a" )
	DISK_IMAGE_READONLY( "starrider", 0, NO_DUMP )
ROM_END

} // anonymous namespace


/***********************************************************************
    Machine definitions
***********************************************************************/
GAME( 1984, starridr, 0, starrider, starrider, sr_state, empty_init, ROT0, "Williams", "Star Rider", MACHINE_IS_SKELETON )
