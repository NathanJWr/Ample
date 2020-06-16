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
#include "ampobject.h"

typedef struct AmpObject_Int {
  AMP_OBJECT_HEADER;
  int val;
} AmpObject_Int;
#define AMP_INTEGER(obj) ((AmpObject_Int *)(obj))
AmpObject *amp_object_create_integer(int val);

AmpObject *amp_integer_add(AmpObject *this, AmpObject *val);
AmpObject *amp_integer_sub(AmpObject *this, AmpObject *val);
AmpObject *amp_integer_div(AmpObject *this, AmpObject *val);
AmpObject *amp_integer_mul(AmpObject *this, AmpObject *val);
