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
#ifndef STR_OBJECT_H_
#define STR_OBJECT_H_
#include "ampobject.h"
typedef struct AmpObject_Str
{
  AMP_OBJECT_HEADER;
  char string[1];
} AmpObject_Str;
#define AMP_STRING(obj) ((AmpObject_Str *)(obj))
AmpObject *amp_object_create_string (const char* str);
AmpObject *amp_object_create_string_nodup (char *str);
void amp_object_destroy_string (AmpObject *obj);

AmpObject *amp_string_concat (AmpObject *this, AmpObject *str);
#endif
