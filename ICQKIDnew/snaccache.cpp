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

#include "snaccache.h"

void SnacCache::addEntry(SNACData & snd)
{
 cache.push_back(pair<SNACData, time_t>(snd, time(NULL)));
}

bool SnacCache::findEntry(SNACData & snd)
{
 for (size_t i=0; i<cache.size(); ++i)
  {
  if ((cache[i].first.service_id==snd.service_id || snd.service_id==0) && \
      (cache[i].first.subtype_id==snd.subtype_id || snd.subtype_id==0) && \
      (cache[i].first.req_id==snd.req_id || snd.req_id==0))
   {
   snd=cache[i].first;
   cache.erase(cache.begin()+i);
   return true;
   }
  }
 
 return false;
}

void SnacCache::clearOlderThan(time_t tmdelta)
{
 for (size_t i=0; i<cache.size(); /* empty */)
  {
  if ((time(NULL)-cache[i].second)>=tmdelta) cache.erase(cache.begin()+i);
  else ++i;
  }
}
