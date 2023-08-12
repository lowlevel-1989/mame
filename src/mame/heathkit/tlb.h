// license:BSD-3-Clause
// copyright-holders:Mark Garlanger
/***************************************************************************

  Heathkit Terminal Logic Board (TLB)

****************************************************************************/

#ifndef MAME_HEATHKIT_TLB_H
#define MAME_HEATHKIT_TLB_H

#pragma once

#include "cpu/z80/z80.h"
#include "machine/clock.h"
#include "machine/ins8250.h"
#include "machine/mm5740.h"
#include "sound/beep.h"
#include "video/mc6845.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


// Standard Heath Terminal logic board
class heath_tlb_device : public device_t
{
public:
	heath_tlb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	// interface routines
	auto serial_data_callback() { return m_write_sd.bind(); }
	auto dtr_callback() { return m_dtr_cb.bind(); }
	auto rts_callback() { return m_rts_cb.bind(); }
	auto reset_cb() { return m_reset.bind(); }

	void serial_in_w(int state);
	void rlsd_in_w(int state);
	void dsr_in_w(int state);
	void cts_int_w(int state);

	void reset_key_w(int state);
	void right_shift_w(int state);
	void repeat_key_w(int state);
	void break_key_w(int state);
	void serial_irq_w(int state);

protected:
	heath_tlb_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock = 0);

	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	void mem_map(address_map &map);
	void io_map(address_map &map);

	virtual MC6845_UPDATE_ROW(crtc_update_row);

	required_device<cpu_device>     m_maincpu;
	required_device<screen_device>  m_screen;
	required_device<palette_device> m_palette;
	required_device<mc6845_device>  m_crtc;
	required_shared_ptr<uint8_t>    m_p_videoram;
	required_region_ptr<uint8_t>    m_p_chargen;

private:
	void set_irq_line();
	void check_for_reset();

	void key_click_w(uint8_t data);
	void bell_w(uint8_t data);
	uint8_t kbd_key_r();
	uint8_t kbd_flags_r();
	uint16_t translate_mm5740_b(uint16_t b);

	void check_beep_state();

	void serial_out_b(int data);
	void dtr_out(int data);
	void rts_out(int data);

	int mm5740_shift_r();
	int mm5740_control_r();
	void mm5740_data_ready_w(int state);

	TIMER_CALLBACK_MEMBER(key_click_off);
	TIMER_CALLBACK_MEMBER(bell_off);

	emu_timer *m_key_click_timer;
	emu_timer *m_bell_timer;

	devcb_write_line m_write_sd;
	devcb_write_line m_dtr_cb;
	devcb_write_line m_rts_cb;
	devcb_write_line m_reset;

	required_device<ins8250_device> m_ace;
	required_device<beep_device>    m_beep;
	required_device<mm5740_device>  m_mm5740;
	required_memory_region          m_kbdrom;
	required_ioport                 m_kbspecial;
	required_device<clock_device>   m_repeat_clock;

	uint8_t  m_transchar;
	bool     m_strobe;
	bool     m_key_click_active;
	bool     m_bell_active;
	bool     m_reset_pending;
	bool     m_right_shift;
	bool     m_reset_key;
	bool     m_keyboard_irq_raised;
	bool     m_serial_irq_raised;
	bool     m_break_key_irq_raised;
};

// Heath TLB with Super19 ROM
class heath_super19_tlb_device : public heath_tlb_device
{
public:
	heath_super19_tlb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
};

// Heath TLB with Watzman (HUG) ROM
class heath_watz_tlb_device : public heath_tlb_device
{
public:
	heath_watz_tlb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
};

// Heath TLB with Ultra ROM
class heath_ultra_tlb_device : public heath_tlb_device
{
public:
	heath_ultra_tlb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	void mem_map(address_map &map);
};

// Heath TLB plus Northwest Digital Systems GP-19
class heath_gp19_tlb_device : public heath_tlb_device
{
public:
	heath_gp19_tlb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;

	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	void mem_map(address_map &map);
	void io_map(address_map &map);

	void latch_u5_w(uint8_t data);

	virtual MC6845_UPDATE_ROW(crtc_update_row) override;

	bool m_char_gen_a11;
	bool m_graphic_mode;
	bool m_col_132;
	bool m_reverse_video;
};

DECLARE_DEVICE_TYPE(HEATH_TLB, heath_tlb_device)
DECLARE_DEVICE_TYPE(HEATH_GP19, heath_gp19_tlb_device)
DECLARE_DEVICE_TYPE(HEATH_SUPER19, heath_super19_tlb_device)
DECLARE_DEVICE_TYPE(HEATH_WATZ, heath_watz_tlb_device)
DECLARE_DEVICE_TYPE(HEATH_ULTRA, heath_ultra_tlb_device)

#endif // MAME_HEATHKIT_TLB_H
