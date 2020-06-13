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
#include "hash.h"
#include "ampobject.h"
DICT_DECLARE (IntVars, const char *, AmpObject *);
DICT_DECLARE (StrVars, const char *, AmpObject *);
/* maps a variable to a particular type */
DICT_DECLARE (Vars, const char *, AmpObjectType);
