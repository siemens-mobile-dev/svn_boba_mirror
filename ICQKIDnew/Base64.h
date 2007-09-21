/***************************************************************************
 *   Copyright (C) 2007 by Alexander S. Salieff                            *
 *   salieff@mail.ru                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _BASE64_H
#define _BASE64_H

#include <string>

class Base64
{
 public:
  Base64();
  void encode(const std::string&, std::string& , bool add_crlf = true);
  void encode(const char* input,size_t l,std::string& output, bool add_crlf = true);
  void decode(const std::string&, std::string& );

  size_t decode_length(const std::string& );

 private:
  Base64(const Base64& ) {}
  Base64& operator=(const Base64& ) { return *this; }
  static char *bstr;
  static char rstr[128];
};

#endif // _BASE64_H
