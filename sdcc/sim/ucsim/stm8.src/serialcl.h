/*
 * Simulator of microcontrollers (serialcl.h)
 *
 * Copyright (C) 1999,99 Drotos Daniel, Talker Bt.
 * 
 * To contact author send email to drdani@mazsola.iit.uni-miskolc.hu
 *
 */

/* This file is part of microcontroller simulator: ucsim.

UCSIM is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

UCSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with UCSIM; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA. */
/*@1@*/

/* $Id: serialcl.h 346 2016-07-10 14:53:32Z  $ */

#ifndef STM8_SERIALCL_HEADER
#define STM8_SERIALCL_HEADER

#include "fiocl.h"
#include "stypes.h"
#include "pobjcl.h"
#include "uccl.h"
#include "newcmdposixcl.h"


class cl_serial_listener;

enum reg_idx {
  sr	= 0,
  dr	= 1,
  brr1	= 2,
  brr2	= 3,
  cr1	= 4,
  cr2	= 5,
  cr3	= 6,
  cr4	= 7,
  cr5	= 8,
  cr6	= 9,
  gtr	= 10,
  pscr	= 11
};

class cl_serial: public cl_hw
{
 protected:
  t_addr base;
  int type;
  class cl_memory_cell *regs[12];
  int div;
  int mcnt;
  bool    sr_read;	// last op was read of SR
  uint8_t s_in;		// Serial channel input reg
  uint8_t s_out;	// Serial channel output reg
  uint8_t s_txd;	// TX data register
  bool    s_sending;	// Transmitter is working (s_out is not empty)
  bool    s_receiving;	// Receiver is working (s_in is shifting)
  bool	  s_tx_written;	// TX data reg has been written
  int     s_rec_bit;	// Bit counter of receiver
  int     s_tr_bit;	// Bit counter of transmitter
  uchar   bits;		// Nr of bits to send/receive
  bool    ren;		// Receiving is enabled
  bool    ten;		// Transmitter is enabled
  bool    en;		// USART is enabled
 public:
  class cl_optref *serial_in_file_option;
  class cl_optref *serial_out_file_option;
  class cl_optref *serial_port_option;
  class cl_serial_listener *listener;
  class cl_f *fin;	// Serial line input
  class cl_f *fout;	// Serial line output
 public:
  cl_serial(class cl_uc *auc,
	    t_addr abase,
	    int ttype);
  virtual ~cl_serial(void);
  virtual int init(void);

  virtual void new_hw_added(class cl_hw *new_hw);
  virtual void added_to_uc(void);
  virtual t_mem read(class cl_memory_cell *cell);
  virtual void write(class cl_memory_cell *cell, t_mem *val);

  virtual int tick(int cycles);
  virtual void start_send();
  virtual void restart_send();
  virtual void finish_send();
  virtual void received();
  virtual void reset(void);
  virtual void new_io(class cl_f *f_in, class cl_f *f_out);

  virtual void pick_div();
  virtual void pick_ctrl();
  virtual void show_writable(bool val);
  virtual void show_readable(bool val);
  virtual void show_tx_complete(bool val);
  virtual void show_idle(bool vol);
  virtual void set_dr(t_mem val);
};



class cl_serial_listener: public cl_listen_console
{
 public:
  class cl_serial *serial_hw;
  cl_serial_listener(int serverport, class cl_app *the_app,
		     class cl_serial *the_serial);
  virtual int proc_input(class cl_cmdset *cmdset);
  virtual bool prevent_quit(void) { return false; }
};


#endif

/* End of stm8.src/serialcl.h */
