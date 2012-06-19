/*  jFUSE-ll provides low level bindings to FUSE in Java 
    Copyright (C) <2011>  <Christopher Brumgard>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package jFUSE.lowlevel;

public class ConnInfo
{
    /* Internal value c pointer */
    protected long fuse_conn_info_ptr = 0;
    
    /* Access methods */
    native public int  getProtoMajor();
    native public int  getProtoMinor();
   
    native public int  getAsyncRead();
    native public void setAsyncRead(int value);
    
    native public int  getMaxWrite();
    native public void setMaxWrite(int value);
    
    native public int  getMaxReadahead();
    native public void setMaxReadahead(int value);
    
    native public int  getCapable();
    native public void setCapable(int value);
    
    native public int  getWant();
    native public void setWant(int value);
}
