/*
    This file is part of Ample.

    Ample is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ample is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ample.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "strobject.h"
#include <stdlib.h>
#include <string.h>
AmpObject *
amp_string_concat (AmpObject *this, AmpObject *str)
{
  unsigned int size = strlen (AMP_STRING (this)->string);
  AmpObject* obj = NULL;
  char *s = NULL;

  size += strlen (AMP_STRING (str)->string);
  s = calloc (1, size+1);
  strcat (s, AMP_STRING (this)->string);
  strcat (s, AMP_STRING (str)->string);

  obj = amp_object_create_string (s);
  free (s);
  return obj;
}

static AmpObjectInfo str_info;
AmpObject *
amp_object_create_string (const char *str)
{
  AmpObject_Str *a = NULL;
  
  str_info.type = AMP_OBJ_STR;
  str_info.add = amp_string_concat;

  a = malloc (sizeof (AmpObject_Str) - 1 + strlen (str) + 1);
  a->refcount = 1;
  a->info = &str_info;
  a->dealloc = amp_object_destroy_basic;
  strcpy (a->string, str);
  return AMP_OBJECT (a);
}

