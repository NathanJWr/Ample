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
#include "boolobject.h"
#include <stdlib.h>
#include <string.h>
AmpObject *
amp_string_concat (AmpObject *this, AmpObject *str)
{
  size_t size = strlen (AMP_STRING (this)->string);
  AmpObject* obj = NULL;
  char *s = NULL;

  size += strlen (AMP_STRING (str)->string);
  s = calloc (1, size+1);
  strcat (s, AMP_STRING (this)->string);
  strcat (s, AMP_STRING (str)->string);

  obj = AmpStringCreate (s);
  free (s);
  return obj;
}

AmpObject *
amp_string_equal (AmpObject *this, AmpObject *str)
{
  const char *string1 = AMP_STRING (this)->string;
  const char *string2 = AMP_STRING (str)->string;
  bool32 equal = 0 == strcmp (string1, string2);
  return AmpBoolCreate (equal);
}

AmpObject *
amp_string_not_equal (AmpObject *this, AmpObject *str)
{
  AmpObject_Bool *equal = AMP_BOOL (amp_string_equal (this, str));
  equal->val = !equal->val;
  return AMP_OBJECT (equal);
}


static AmpObjectInfo str_info;
static bool32 str_info_initialized;
AmpObject *
AmpStringCreate (const char *str)
{
  AmpObject_Str *a = NULL;
  
  if (!str_info_initialized)
    {
      str_info.type = AMP_OBJECT_STRING;
      AmpObjectInitializeOperationsToUnsupported (&str_info.ops);
      str_info.ops.add = amp_string_concat;
      str_info.ops.equal = amp_string_equal;
      str_info.ops.not_equal = amp_string_not_equal;
      str_info_initialized = true;
    }

  a = malloc (sizeof (AmpObject_Str) - 1 + strlen (str) + 1);
  a->refcount = 1;
  a->info = &str_info;
  a->dealloc = AmpObjectDestroyBasic;
  strcpy (a->string, str);
  return AMP_OBJECT (a);
}

