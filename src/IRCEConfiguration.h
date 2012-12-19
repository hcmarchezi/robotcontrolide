/***************************************************************************
 *   Copyright (C) 2007 by Humberto Cardoso Marchezi   *
 *   hcmarchezi@linux   *
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
#ifndef IRCECONFIGURATION_H
#define IRCECONFIGURATION_H

#include<string>

using namespace std;

/**
	@author Humberto Cardoso Marchezi <hcmarchezi@linux>
*/
class IRCEConfiguration
{
public: static string strPlayerPath;
public: static string strGazeboPath;
public: static string strCPPCompilerPath;
public: static string strPlayerHeaderPath;
public: static string strPlayerLibPath;
public: static string strPlayerLibs;
};

#endif
