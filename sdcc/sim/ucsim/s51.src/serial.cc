/*
 * Simulator of microcontrollers (serial.cc)
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

#include "ddconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <strings.h>

// prj
#include "globals.h"
#include "utils.h"

// cmd
#include "cmdutil.h"

// local
#include "serialcl.h"
#include "regs51.h"
#include "uc51cl.h"


cl_serial::cl_serial(class cl_uc *auc):
  cl_hw(auc, HW_UART, 0, "uart")
{
  fin= 0;
  fout= 0;
  listener= 0;
}

cl_serial::~cl_serial(void)
{
  if (fout)
    delete fout;
  if (fin)
    delete fin;
  delete serial_in_file_option;
  delete serial_out_file_option;
}

int
cl_serial::init(void)
{
  char *s;

  cl_hw::init();
  set_name("mcs51_uart");
  sfr= uc->address_space(MEM_SFR_ID);
  bas= uc->address_space("bits");
  if (sfr)
    {
      sbuf= register_cell(sfr, SBUF);
      pcon= register_cell(sfr, PCON);
      scon= register_cell(sfr, SCON);
    }
  int i;
  for (i= 0; i < 8; i++)
    {
      scon_bits[i]= register_cell(bas, SCON+i);
    }
  
  s= format_string("serial%d_in_file", id);
  serial_in_file_option= new cl_optref(this);
  serial_in_file_option->init();
  serial_in_file_option->use(s);
  free(s);
  s= format_string("serial%d_out_file", id);
  serial_out_file_option= new cl_optref(this);
  serial_out_file_option->init();
  serial_out_file_option->use(s);
  free(s);
  s= format_string("serial%d_port", id);
  serial_port_option= new cl_optref(this);
  serial_port_option->init();
  class cl_option *o= serial_port_option->use(s);
  free(s);
  if (o)
    {
      int port= serial_port_option->get_value((long)0);
      if (port > 0)
	listener= new cl_serial_listener(port, application, this);
      class cl_commander_base *c= application->get_commander();
      c->add_console(listener);
    }
  
  char *f_serial_in = (char*)serial_in_file_option->get_value((char*)0);
  char *f_serial_out= (char*)serial_out_file_option->get_value((char*)0);
  if (f_serial_in)
    {
      if (f_serial_in[0] == '\001')
	fin= (class cl_f *)(strtoll(&f_serial_in[1], 0, 0));
      else
	fin= mk_io(chars(f_serial_in), cchars("r"));
      //fin->save_attributes();
      fin->set_terminal();
      if (!fin->tty)
	fprintf(stderr, "Warning: serial input interface connected to a "
		"non-terminal file.\n");
    }
  else
    fin= mk_io(chars(""), chars(""));
  if (f_serial_out)
    {
      if (f_serial_out[0] == '\001')
	fout= (class cl_f *)(strtoll(&f_serial_out[1], 0, 0));
      else
	fout= mk_io(chars(f_serial_out), "w");
      //fout->save_attributes();
      fout->set_terminal();
      if (!fout->tty)
	fprintf(stderr, "Warning: serial output interface connected to a "
		"non-terminal file.\n");
    }
  else
    fout= mk_io(chars(""), chars(""));
  class cl_hw *t2= uc->get_hw(HW_TIMER, 2, 0);
  if ((there_is_t2= t2 != 0))
    {
      t_mem d= sfr->get(T2CON);
      t2_baud= d & (bmRCLK | bmTCLK);
    }
  else
    t2_baud= false;

  return(0);
}

void
cl_serial::new_hw_added(class cl_hw *new_hw)
{
  if (new_hw->cathegory == HW_TIMER &&
      new_hw->id == 2)
    {
      there_is_t2= true;
      t_mem d= sfr->get(T2CON);
      t2_baud= d & (bmRCLK | bmTCLK);
    }
}

void
cl_serial::added_to_uc(void)
{
  class cl_address_space *sfr= uc->address_space(MEM_SFR_ID);

  uc->it_sources->add(new cl_it_src(uc, bmES,
				    sfr->get_cell(IE), bmES,
				    sfr->get_cell(SCON), bmTI,
				    0x0023, false, false,
				    "serial transmit", 6));
  uc->it_sources->add(new cl_it_src(uc, bmES,
				    sfr->get_cell(IE), bmES,
				    sfr->get_cell(SCON), bmRI,
				    0x0023, false, false,
				    "serial receive", 6));
}

t_mem
cl_serial::read(class cl_memory_cell *cell)
{
  if (cell == sbuf)
    return(s_in);
  else
    return(cell->get());
}

void
cl_serial::write(class cl_memory_cell *cell, t_mem *val)
{
  t_addr ba;
  bool b= bas->is_owned(cell, &ba);
  uint8_t n= *val;
  
  if (cell == sbuf)
    {
      s_out= *val;
      s_sending= true;
      s_tr_bit = 0;
      s_tr_tick= 0;
      s_tr_t1= 0;
    }
  if (b)
    {
      n= scon->get();
      uint8_t m= 1 << (ba - SCON);
      if (*val)
	n|= m;
      else
	n&= ~m;
    }
  if ((cell == scon) ||
      b)
    {
      _mode= n >> 6;
      _bmREN= n & bmREN;
      _bits= 8;
      switch (_mode)
	{
	case 0:
	  _bits= 8;
	  _divby= 12;
	  break;
	case 1:
	  _bits= 10;
	  _divby= _bmSMOD?16:32;
	  break;
	case 2:
	  _bits= 11;
	  _divby= _bmSMOD?16:32;
	  break;
	case 3:
	  _bits= 11;
	  _divby= _bmSMOD?16:32;
	  break;
	}
    }
  else if (cell == pcon)
    {
      _bmSMOD= *val & bmSMOD;
      /*switch (_mode)
	{
	case 1:
	  _divby= _bmSMOD?16:32;
	  break;
	case 2:
	  _divby= _bmSMOD?16:32;
	  break;
	case 3:
	  _divby= _bmSMOD?16:32;
	  break;
	  }*/
      if (_mode)
	_divby= _bmSMOD?16:32;
    }
}

int
cl_serial::serial_bit_cnt(void)
{
  //int divby= 12;
  int *tr_src= 0, *rec_src= 0;

  switch (_mode)
    {
    case 0:
      //divby  = 12;
      tr_src = &s_tr_tick;
      rec_src= &s_rec_tick;
      break;
    case 1:
    case 3:
      //divby  = (/*pcon->get()&bmSMOD*/_bmSMOD)?16:32;
      tr_src = &s_tr_t1;
      rec_src= &s_rec_t1;
      break;
    case 2:
      //divby  = (/*pcon->get()&bmSMOD*/_bmSMOD)?16:32;
      tr_src = &s_tr_tick;
      rec_src= &s_rec_tick;
      break;
    }
  if (t2_baud)
    _divby= 16;
  if (s_sending)
    {
      while (*tr_src >= _divby)
	{
	  (*tr_src)-= _divby;
	  s_tr_bit++;
	}
    }
  if (s_receiving)
    {
      while (*rec_src >= _divby)
	{
	  (*rec_src)-= _divby;
	  s_rec_bit++;
	}
    }
  return(0);
}

int
cl_serial::tick(int cycles)
{
  char c;

  serial_bit_cnt(/*_mode*/);
  if (s_sending &&
      (s_tr_bit >= _bits))
    {
      s_sending= false;
      scon->set_bit1(bmTI);
      if (fout)
	{
	  fout->write((char*)(&s_out), 1);
	}
      s_tr_bit-= _bits;
    }
  if ((_bmREN) &&
      fin &&
      !s_receiving)
    {
      if (fin->input_avail())
	{
	  s_receiving= true;
	  s_rec_bit= 0;
	  s_rec_tick= s_rec_t1= 0;
	}
    }
  if (s_receiving &&
      (s_rec_bit >= _bits))
    {
      if (fin->read(&c, 1) == 1)
	{
	  s_in= c;
	  sbuf->set(s_in);
	  received(c);
	}
      s_receiving= false;
      s_rec_bit-= _bits;
    }
  
  int l;
  s_tr_tick+= (l= cycles * uc->clock_per_cycle());
  s_rec_tick+= l;
  return(0);
}

void
cl_serial::received(int c)
{
  scon->set_bit1(bmRI);
}

void
cl_serial::reset(void)
{
  s_tr_t1    = 0;
  s_rec_t1   = 0;
  s_tr_tick  = 0;
  s_rec_tick = 0;
  s_in       = 0;
  s_out      = 0;
  s_sending  = false;
  s_receiving= false;
  s_rec_bit  = 0;
  s_tr_bit   = 0;
}

void
cl_serial::happen(class cl_hw *where, enum hw_event he, void *params)
{
  if (where->cathegory == HW_TIMER)
    {
      if (where->id == 1)
	{
	  s_rec_t1++;
	  s_tr_t1++;
	}
      if (where->id == 2 /*&& there_is_t2*/)
	{
	  switch (he)
	    {
	    case EV_T2_MODE_CHANGED:
	      {
		if (!t2_baud)
		  s_rec_t1= s_tr_t1= 0;
		t_mem *d= (t_mem *)params;
		t2_baud= *d & (bmRCLK | bmTCLK);
		break;
	      }
	    case EV_OVERFLOW:
	      s_rec_t1++;
	      s_tr_t1++;
	      break;
	    default: break;
	    }
	}
    }
}


void
cl_serial::new_io(class cl_f *f_in, class cl_f *f_out)
{
  if (fin)
    delete fin;
  if (fout)
    delete fout;
  fin= f_in;
  fout= f_out;
  fin->set_terminal();
  fout->set_terminal();
  //printf("usart[%d] now using fin=%d fout=%d\n", id, fin->file_id, fout->file_id);
}


void
cl_serial::print_info(class cl_console_base *con)
{
  const char *modes[]= { "Shift, fixed clock",
			 "8 bit UART timer clocked",
			 "9 bit UART fixed clock",
			 "9 bit UART timer clocked" };
  int sc= scon->get();

  con->dd_printf("%s[%d]", id_string, id);
  int mode= (sc&(bmSM0|bmSM1))>>6;
  con->dd_printf(" %s", modes[mode]);
  if (mode == 1 || mode == 2)
    con->dd_printf(" (timer%d)", (t2_baud)?2:1);
  con->dd_printf(" MultiProc=%s",
		 (mode&2)?((sc&bmSM2)?"ON":"OFF"):"none");
  con->dd_printf(" irq=%s", (sfr->get(IE)&bmES)?"en":"dis");
  con->dd_printf(" prio=%d", uc->priority_of(bmPS));
  con->dd_printf("\n");

  con->dd_printf("Receiver");
  con->dd_printf(" %s", (sc&bmREN)?"ON":"OFF");
  con->dd_printf(" RB8=%c", (sc&bmRB8)?'1':'0');
  con->dd_printf(" irq=%c", (sc&bmRI)?'1':'0');
  con->dd_printf("\n");

  con->dd_printf("Transmitter");
  con->dd_printf(" TB8=%c", (sc&bmTB8)?'1':'0');
  con->dd_printf(" irq=%c", (sc&bmTI)?'1':'0');
  con->dd_printf("\n");
  /*con->dd_printf("s_rec_t1=%d s_rec_bit=%d s_rec_tick=%d\n",
		 s_rec_t1, s_rec_bit, s_rec_tick);
  con->dd_printf("s_tr_t1=%d s_tr_bit=%d s_tr_tick=%d\n",
		 s_tr_t1, s_tr_bit, s_tr_tick);
		 con->dd_printf("divby=%d bits=%d\n", _divby, _bits);*/
}


cl_serial_listener::cl_serial_listener(int serverport, class cl_app *the_app,
				       class cl_serial *the_serial):
  cl_listen_console(serverport, the_app)
{
  serial_hw= the_serial;
}

int
cl_serial_listener::proc_input(class cl_cmdset *cmdset)
{
  class cl_f *i, *o;

  srv_accept(fin, &i, &o);
  serial_hw->new_io(i, o);
  return 0;
}


/* End of s51.src/serial.cc */
