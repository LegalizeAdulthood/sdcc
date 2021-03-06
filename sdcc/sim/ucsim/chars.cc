/*
 * Simulator of microcontrollers (chars.cc)
 *
 * Copyright (C) 1997,16 Drotos Daniel, Talker Bt.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "charscl.h"


chars::chars(void)
{
  chars_string= 0;
  chars_length= 0;
}

chars::chars(char *s)
{
  chars_string= 0;
  chars_length= 0;
  allocate_string(s);
}

chars::chars(const char *s)
{
  chars_string= 0;
  chars_length= 0;
  allocate_string((char*)s);
}

chars::chars(const chars &cs)
{
  chars_string= 0;
  chars_length= 0;
  allocate_string((char*)cs);
}

chars::~chars(void)
{
  deallocate_string();
}


void
chars::allocate_string(char *s)
{
  deallocate_string();
  if (s)
    {
      chars_length= strlen(s);
      chars_string= (char*)malloc(chars_length+1);//new char[chars_length+1];
      strcpy(chars_string, s);
    }
}

void
chars::deallocate_string(void)
{
  if (chars_string)
    free(chars_string);//delete [] chars_string;
  chars_string= 0;
  chars_length= 0;
}


chars &
chars::append(char *s)
{
  if (!s)
    return(*this);

  char *temp= new char[chars_length + strlen(s) + 1];
  if (chars_string)
    strcpy(temp, chars_string);
  else
    temp[0]= '\0';
  strcat(temp, s);
  //allocate_string(temp);
  chars_string= temp;
  chars_length+= strlen(s);
  //delete [] temp;

  return *this;
}

chars &
chars::append(char c)
{
  if (!c)
    return(*this);

  char *temp= new char[chars_length + 1 + 1];
  if (chars_string)
    strcpy(temp, chars_string);
  else
    temp[0]= '\0';
  char b[2];
  b[0]= c;
  b[1]= 0;
  strcat(temp, b);
  //allocate_string(temp);
  chars_string= temp;
  chars_length+= 1;
  //delete [] temp;

  return *this;
}

chars &
chars::append(const char *format, ...)
{
  va_list ap;
  char n[1000];
  
  va_start(ap, format);
  vsnprintf(n, 999, format, ap);
  va_end(ap);

  char *temp= new char[chars_length + strlen(n) + 1];
  if (chars_string)
    strcpy(temp, chars_string);
  else
    temp[0]= '\0';
  strcat(temp, n);
  chars_string= temp;
  chars_length+= strlen(n);

  return *this;
}

bool
chars::empty()
{
  return chars_length == 0;
}

// Assignment operators
chars &
chars::operator=(char *s)
{
  allocate_string(s);
  return(*this);
}

chars &
chars::operator=(const chars &cs)
{
  allocate_string((char*)cs);
  return(*this);
}


// Arithmetic operators

chars
chars::operator+(char c)
{
  char b[2];
  b[0]= c;
  b[1]= 0;
  chars temp(chars_string);
  return temp.append(b);
}

chars
chars::operator+(char *s)
{
  chars temp(chars_string);
  return(temp.append(s));
}

chars
chars::operator+(const chars &cs)
{
  chars temp(chars_string);
  return(temp.append(cs));
}

chars
operator+(char c, const chars &cs)
{
  char b[2];
  b[0]= c;
  b[1]= 0;
  chars temp(b);
  return(temp.append((char*)cs));
};

chars
operator+(char *s, const chars &cs)
{
  chars temp(s);
  return(temp.append((char*)cs));
};


// Boolean operators
bool
chars::equal(char *s)
{
  if ((chars_string &&
       !s) ||
      (!chars_string &&
       s))
    return(0);
  if (!chars_string &&
      !s)
    return(1);
  return(strcmp(chars_string, s) == 0);
}

bool
chars::operator==(char *s)
{
  return(equal(s));
}

bool
chars::operator==(const char *s)
{
  return(equal((char*)s));
}

bool
chars::operator==(chars &cs)
{
  return(*this == (char*)cs);
}

bool
operator==(char *s, const chars &cs)
{
  return((chars(cs)).equal(s));
}

bool
operator==(const char *s, const chars &cs)
{
  return((chars(cs)).equal((char*)s));
}

bool
chars::operator!=(char *s)
{
  return(!equal(s));
}

bool
chars::operator!=(const char *s)
{
  return(!equal((char*)s));
}

bool
chars::operator!=(chars &cs)
{
  return(*this != (char*)cs);
}

bool
operator!=(char *s, const chars &cs)
{
  return(!(chars(cs)).equal(s));
}

bool
operator!=(const char *s, const chars &cs)
{
  return(!(chars(cs)).equal((char*)s));
}

/*
cchars::cchars(const char *s):
chars(s)
{
}
*/

cchars::cchars(char const *s):
chars(s)
{
}

cchars::~cchars(void)
{
  deallocate_string();
}

void
cchars::allocate_string(const char *s)
{
  deallocate_string();
  /*if (s)
    {
      chars_length= strlen(s);
      chars_string= s
      }*/
  allocate_string((char*)s);
}

void
cchars::deallocate_string(void)
{
  chars_string= 0;
  chars_length= 0;
}


/* End of chars.cc */
