/* ************************************************************************ */
/*																			*/
/*  Neko-Aspell - a Neko VM library that wraps GNU Aspell					*/
/*  Copyright (c)2006,2007													*/
/*  Edwin van Rijkom														*/
/*																			*/
/*  The development of this library was kindly sponsored by Artifex			*/
/*  (www.afxfirm.com)														*/
/*																			*/
/* This library is free software; you can redistribute it and/or			*/
/* modify it under the terms of the GNU Lesser General Public				*/
/* License as published by the Free Software Foundation; either				*/
/* version 2.1 of the License, or (at your option) any later version.		*/
/*																			*/
/* This library is distributed in the hope that it will be useful,			*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of			*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*/
/* Lesser General Public License or the LICENSE file for more details.		*/
/*																			*/
/* ************************************************************************ */

package aspell;

class Config {
	
	var c : Void;
	
	static var _nas_new_config = neko.Lib.load("aspell","nas_new_config",0);
	public function new() {
		c = _nas_new_config();
	}
	
	static var _nas_config_replace = neko.Lib.load("aspell","nas_config_replace",3);
	public function replace(varname: String, val: String) {
		_nas_config_replace(c,untyped varname.__s,untyped val.__s);
	}
	
	static var _nas_new_speller = neko.Lib.load("aspell","nas_new_speller",1);
	public function createSpeller(): Error {
		return new Error(_nas_new_speller(c),c);
	}
}

