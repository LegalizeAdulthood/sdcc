/*@1@*/

#include "varcl.h"


cl_var::cl_var(char *iname, class cl_address_space *ias, t_addr iaddr, int ibit):
  cl_base()
{
  as= ias;
  addr= iaddr;
  bit= ibit;

  set_name(iname);
  
  cell= NULL;
}

int
cl_var::init(void)
{
  if (!as ||
      !as->is_address_space() ||
      !as->valid_address(addr))
    return 0;
  cell= as->get_cell(addr);
  if (cell && (bit < 0))
    cell->set_flag(CELL_VAR, true);
  return 0;
}


void
cl_var::print_info(cl_console_base *con)
{
  con->dd_printf("%s ", get_name("?"));
  if (cell)
    {
      con->dd_printf("%s", as->get_name("?"));
      con->dd_printf("[");
      con->dd_printf(as->addr_format, addr);
      con->dd_printf("]");
      if (bit >= 0)
	con->dd_printf(".%d", bit);
    }
  con->dd_printf("\n");
}


void *
cl_var_list::key_of(void *item)
{
  class cl_var *v= (class cl_var *)item;
  return (void*)v->get_name();
}

int
cl_var_list::compare(void *key1, void *key2)
{
  char *k1, *k2;

  k1= (char*)key1;
  k2= (char*)key2;
  if (k1 && k2)
    return strcmp(k1, k2);
  return 0;
}


/* End of sim.src/var.cc */
