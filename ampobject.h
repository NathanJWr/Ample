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
#ifndef AMP_OBJECT_H_
#define AMP_OBJECT_H_
typedef enum AmpObjectType
{
  AMP_OBJ_INT,
  AMP_OBJ_STR,
} AmpObjectType;

#define AMP_OBJECT_HEADER                                                     \
  unsigned int refcount;                                                      \
  AmpObjectType type;                                                         \
  void (*dealloc) ();                                                         \
  void *value

typedef struct AmpObject
{
  AMP_OBJECT_HEADER;
} AmpObject;
#define AMP_OBJECT(obj) ((AmpObject*)(obj))
void obj_inc_refcount (AmpObject *obj);
void obj_dec_refcount (AmpObject *obj);
void amp_object_destroy_basic (AmpObject *obj);
#endif // AMP_OBJECT_H_
