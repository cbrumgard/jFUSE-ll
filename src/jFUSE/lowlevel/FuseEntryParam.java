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

public class FuseEntryParam
{
    /* Instance variables */
    long      ino;
    long      generation;
    FuseStat  attr;
    double    attr_timeout;
    double    entry_timeout;
    
    public FuseEntryParam(long ino, long generation, FuseStat attr, 
            double attr_timeout, double entry_timeout)
    {
        /* Assigns the values */
        this.ino           = ino;
        this.generation    = generation;
        this.attr          = attr;
        this.attr_timeout  = attr_timeout;
        this.entry_timeout = attr_timeout;
    }
}