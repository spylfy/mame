// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/******************************************************************************
*
*   Sony Playstation 2 IOP DMA device skeleton
*
*   To Do:
*     Everything
*
*/

#ifndef MAME_MACHINE_IOPDMA_H
#define MAME_MACHINE_IOPDMA_H

#pragma once

#include "emu.h"
#include "ps2sif.h"
#include "iopintc.h"
#include "audio/iopspu.h"

class iop_dma_device : public device_t, public device_execute_interface
{
public:
	template <typename T, typename U, typename V, typename W, typename X>
    iop_dma_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&intc_tag, U &&ram_tag, V &&sif_tag, W &&spu_tag, X &&spu2_tag)
    	: iop_dma_device(mconfig, tag, owner, clock)
    {
		m_intc.set_tag(std::forward<T>(intc_tag));
		m_ram.set_tag(std::forward<U>(ram_tag));
		m_sif.set_tag(std::forward<V>(sif_tag));
		m_spu.set_tag(std::forward<W>(spu_tag));
		m_spu2.set_tag(std::forward<X>(spu2_tag));
	}

    iop_dma_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ32_MEMBER(bank0_r);
	DECLARE_WRITE32_MEMBER(bank0_w);
	DECLARE_READ32_MEMBER(bank1_r);
	DECLARE_WRITE32_MEMBER(bank1_w);

	enum channel_type : uint32_t
	{
		MDEC_IN = 0,
		MDEC_OUT,
		GPU,
		CDVD,
		SPU,
		PIO,
		OTC,
		UNUSED_BANK0,
		SPU2,
		UNKNOWN0,
		SIF0,
		SIF1,
		SIO2_IN,
		SIO2_OUT,
		UNKNOWN1,
		UNUSED_BANK1
	};

protected:
	struct intctrl_t
	{
		uint8_t m_mask;
		uint8_t m_status;
		bool m_enabled;
	};

	class channel_t
	{
		friend class iop_dma_device;

	public:
		channel_t()
			: m_priority(0), m_enabled(false), m_addr(0), m_block(0), m_ctrl(0), m_tag_addr(0), m_size(0), m_count(0)
		{
		}

		void set_pri_ctrl(uint32_t pri_ctrl);
		void set_addr(uint32_t addr) { m_addr = addr; }
		void set_block(uint32_t block, uint32_t mem_mask);
		void set_size(uint32_t size) { m_size = size; }
		void set_count(uint32_t count) { m_count = count; }
		void set_ctrl(uint32_t ctrl);
		void set_tag_addr(uint32_t tag_addr) { m_tag_addr = tag_addr; }

		bool enabled() const { return m_enabled; }
		bool busy() const { return m_busy; }
		uint32_t addr() const { return m_addr; }
		uint32_t block() const { return m_block; }
		uint32_t size() const { return m_size; }
		uint32_t count() const { return m_count; }
		uint32_t ctrl() const { return m_ctrl; }
		uint32_t tag_addr() const { return m_tag_addr; }
		bool end() const { return m_end; }

	protected:
		uint8_t m_priority;
		bool m_enabled;
		bool m_busy;
		bool m_end;

		uint32_t m_addr;
		uint32_t m_block;
		uint32_t m_ctrl;
		uint32_t m_tag_addr;

		uint32_t m_size;
		uint32_t m_count;
	};

    virtual void device_start() override;
    virtual void device_reset() override;
	virtual void execute_run() override;

	void set_dpcr(uint32_t data, uint32_t index);
	void set_dicr(uint32_t data, uint32_t index);
	void update_interrupts();

	void transfer_sif0(uint32_t chan);
	void transfer_sif1(uint32_t chan);
	void transfer_spu(uint32_t chan);
	void transfer_finish(uint32_t chan);

	required_device<iop_intc_device> m_intc;
	required_shared_ptr<uint32_t> m_ram;
	required_device<ps2_sif_device> m_sif;
	required_device<iop_spu_device> m_spu;
	required_device<iop_spu_device> m_spu2;

	int m_icount;

	uint32_t m_dpcr[2];
	uint32_t m_dicr[2];
	channel_t m_channels[16];
	intctrl_t m_int_ctrl[2];

	uint32_t m_running_mask;
	uint32_t m_last_serviced;
};

DECLARE_DEVICE_TYPE(SONYIOP_DMA, iop_dma_device)

#endif // MAME_MACHINE_IOPDMA_H
