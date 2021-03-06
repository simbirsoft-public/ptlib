/*
 * ipdsock.h
 *
 * IP Datagram socket I/O channel class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 25756 $
 * $Author: csoutheren $
 * $Date: 2011-05-12 01:07:24 -0500 (Thu, 12 May 2011) $
 */

#ifndef PTLIB_IPDATAGRAMSOCKET_H
#define PTLIB_IPDATAGRAMSOCKET_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/ipsock.h>

/** Internet Protocol Datagram Socket class.
*/
class PIPDatagramSocket : public PIPSocket
{
  PCLASSINFO(PIPDatagramSocket, PIPSocket);
  protected:
    /**Create a TCP/IP protocol socket channel. If a remote machine address or
       a "listening" socket is specified then the channel is also opened.
     */
    PIPDatagramSocket();


  public:
  // New functions for class
    /**Read a datagram from a remote computer.
       
       @return true if any bytes were sucessfully read.
     */
    virtual PBoolean ReadFrom(
      void * buf,     ///< Data to be written as URGENT TCP data.
      PINDEX len,     ///< Number of bytes pointed to by <code>buf</code>.
      Address & addr, ///< Address from which the datagram was received.
      WORD & port     ///< Port from which the datagram was received.
    );
    virtual PBoolean ReadFrom(
      void * buf,     ///< Data to be written as URGENT TCP data.
      PINDEX len,     ///< Number of bytes pointed to by <code>buf</code>.
      PIPSocketAddressAndPort & ipAndPort
    );

    virtual PBoolean ReadFrom(
      Slice * slices,         ///< Data to be written as URGENT TCP data.
      size_t sliceCount,
      Address & addr,         ///< Address from which the datagram was received.
      WORD & port             ///< Port from which the datagram was received.
    );
    virtual PBoolean ReadFrom(
      Slice * slices,         ///< Data to be written as URGENT TCP data.
      size_t sliceCount,
      PIPSocketAddressAndPort & ipAndPort
    );

    /**Write a datagram to a remote computer.

       @return true if all the bytes were sucessfully written.
     */
    virtual PBoolean WriteTo(
      const void * buf,     ///< Data to be written as URGENT TCP data.
      PINDEX len,           ///< Number of bytes pointed to by <code>buf</code>.
      const Address & addr, ///< Address to which the datagram is sent.
      WORD port             ///< Port to which the datagram is sent.
    );
    virtual PBoolean WriteTo(
      const void * buf,     ///< Data to be written as URGENT TCP data.
      PINDEX len,           ///< Number of bytes pointed to by <code>buf</code>.
      const PIPSocketAddressAndPort & ipAndPort
    );

    virtual PBoolean WriteTo(
      const Slice * slices,         ///< Data to be written as URGENT TCP data.
      size_t sliceCount,
      const Address & addr,         ///< Address to which the datagram is sent.
      WORD port                     ///< Port to which the datagram is sent.
    );
    virtual PBoolean WriteTo(
      const Slice * slices,         ///< Data to be written as URGENT TCP data.
      size_t sliceCount,
      const PIPSocketAddressAndPort & ipAndPort
    );


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/ipdsock.h"
#else
#include "unix/ptlib/ipdsock.h"
#endif

  public:
  
    // Normally, one would expect these to be protected, but they are just so darn
    // useful that it's just easier if they are public
    virtual bool InternalReadFrom(
      Slice * slices, 
      size_t sliceCount, 
      PIPSocketAddressAndPort & ipAndPort
    );

    virtual bool InternalWriteTo(
      const Slice * slices, 
      size_t sliceCount, 
      const PIPSocketAddressAndPort & ipAndPort
    );
};


#endif // PTLIB_IPDATAGRAMSOCKET_H


// End Of File ///////////////////////////////////////////////////////////////
