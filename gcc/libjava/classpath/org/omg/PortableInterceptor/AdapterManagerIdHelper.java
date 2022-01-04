/* AdapterManagerIdHelper.java --
   Copyright (C) 2005 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package org.omg.PortableInterceptor;

import gnu.CORBA.OrbRestricted;

import org.omg.CORBA.Any;
import org.omg.CORBA.BAD_OPERATION;
import org.omg.CORBA.ORB;
import org.omg.CORBA.TCKind;
import org.omg.CORBA.TypeCode;
import org.omg.CORBA.portable.InputStream;
import org.omg.CORBA.portable.OutputStream;

/**
 * A helper operations for the adapter manager id. An adapter manager id is an
 * integer constant and needs no helper, but the one is included anyway.
 *
 * @since 1.5
 *
 * @author Audrius Meskauskas, Lithuania (AudriusA@Bioinformatics.org)
 */
public abstract class AdapterManagerIdHelper
{
  /**
   * Create the AdapterManagerId typecode (alias of CORBA long (java int), named
   * "AdapterManagerId").
   */
  public static TypeCode type()
  {
    ORB orb = OrbRestricted.Singleton;
    return orb.create_alias_tc(id(), "AdapterManagerId",
                               orb.get_primitive_tc(TCKind.tk_long));
  }

  /**
   * Insert the int into the given Any.
   */
  public static void insert(Any any, int that)
  {
    any.insert_long(that);
  }

  /**
   * Extract the int from given Any.
   *
   * @throws BAD_OPERATION if the passed Any does not contain int.
   */
  public static int extract(Any any)
  {
    return any.extract_long();
  }

  /**
   * Get the adapter manager id repository id.
   *
   * @return "IDL:omg.org/PortableInterceptor/AdapterManagerId:1.0", always.
   */
  public static String id()
  {
    return "IDL:omg.org/PortableInterceptor/AdapterManagerId:1.0";
  }

  /**
   * Read the int (adapter manager id) from the CDR intput stream.
   *
   * @param input a org.omg.CORBA.portable stream to read from.
   */
  public static int read(InputStream input)
  {
    return input.read_long();
  }

  /**
   * Write the int (adapter manager id) to the CDR output stream.
   *
   * @param output a org.omg.CORBA.portable stream stream to write into.
   * @param value a value to write.
   */
  public static void write(OutputStream output, int value)
  {
    output.write_long(value);
  }
}
