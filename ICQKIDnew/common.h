/***************************************************************************
 *   Copyright (C) 2007 by Andrey "Ice" Sploshnov                          *
 *   ice.nightcrawler@gmail.com                                            *
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

#ifndef ICQKID_COMMON_H
#define ICQKID_COMMON_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef _WIN32
 typedef unsigned char uint8_t;
 typedef unsigned short uint16_t;
 typedef unsigned long uint32_t;
#elif defined STDC_HEADERS
# include <stdint.h>
#else
# include <sys/types.h>
#endif

#endif /* ICQKID_COMMON_H */
