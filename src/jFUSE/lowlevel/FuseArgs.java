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

import java.util.Arrays;

public class FuseArgs
{
    protected long fuse_args_ptr = 0;
    
    
    native protected void _initializeFuseArgs(String[] args);
    native protected void _freeFuseArgs();
    
    
    
    
    public FuseArgs()
    {
        /* Initializes the fuse args */
        _initializeFuseArgs(null);
    }
    
    public FuseArgs(String[] args)
    {
        String[] tmpArgs = new String[args.length+1];
        
        tmpArgs[0] = "java";
        for(int i=0; i<args.length; i++)
        {
            tmpArgs[i+1] = args[i];
        }
        
        
        /* Initializes the fuse args */
        _initializeFuseArgs(tmpArgs);
    }
    
    
    native public void addArg(String arg);
    //native public void optParse(FuseOpt[] myfsOpts);
    
   
    protected void finalize() throws Throwable
    {
        
        _freeFuseArgs();
        
        // TODO Auto-generated method stub
        super.finalize();
    }
}
