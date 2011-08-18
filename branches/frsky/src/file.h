#ifndef file_h
/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define file_h

#include <inttypes.h>


/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)
#if defined (FRSKY)
#define FILE_FRSKY    (FILE_TMP+1)
#endif

bool EeFsOpen();
int8_t EeFsck();
void EeFsFormat();
uint16_t EeFsGetFree();
#define ERR_NONE 0
#define ERR_FULL 1
#define ERR_TMO  2
class EFile
{
  uint8_t  m_fileId;    //index of file in directory = filename
  uint16_t m_pos;       //over all filepos
  uint8_t  m_currBlk;   //current block.id
  uint8_t  m_ofs;       //offset inside of the current block
  uint8_t  m_bRlc;      //control byte for run length decoder
  uint8_t  m_zeroes;    //control byte for run length decoder
  uint8_t  m_err;       //error reasons
  uint16_t m_stopTime10ms; //maximum point of time for writing
public:
  ///remove contents of given file
  static void rm(uint8_t i_fileId); 

  ///swap contents of file1 with them of file2
  static void swap(uint8_t i_fileId1,uint8_t i_fileId2); 

  ///return true if the file with given fileid exists
  static bool exists(uint8_t i_fileId); 

  ///open file for reading, no close necessary
  ///for writing use writeRlc() or create()
  uint8_t openRd(uint8_t i_fileId); 
  /// create a new file with given fileId, 
  /// !!! if this file already exists, then all blocks are reused
  /// and all contents will be overwritten.
  /// after writing closeTrunc has to be called
  void    create(uint8_t i_fileId, uint8_t typ, uint16_t maxTme10ms);
  /// close file and truncate the blockchain if to long.
  void    closeTrunc();

  ///open file, write to file and close it. 
  ///If file existed before, then contents is overwritten. 
  ///If file was larger before, then unused blocks are freed
  uint16_t writeRlc(uint8_t i_fileId, uint8_t typ,uint8_t*buf,uint16_t i_len, uint8_t maxTme10ms);

  uint8_t read(uint8_t*buf,uint16_t i_len);
  uint8_t write(uint8_t*buf,uint8_t i_len);
  uint8_t write1(uint8_t b);

  ///return size of compressed file without block overhead
  uint16_t size(); 
  ///read from opened file and decode rlc-coded data
#ifdef TRANSLATIONS
  uint16_t readRlc12(uint8_t*buf,uint16_t i_len,bool rlc2);
  inline uint16_t readRlc1(uint8_t*buf,uint16_t i_len)
  {
    return readRlc12(buf,i_len,false);
  }
  inline uint16_t readRlc(uint8_t*buf, uint16_t i_len)
  {
    return readRlc12(buf,i_len,true);
  }
#else
  uint16_t readRlc(uint8_t*buf, uint16_t i_len);
#endif
  ///deliver current errno, this is reset in open
  uint8_t errno(){return m_err;}
};

#endif
/*eof*/
