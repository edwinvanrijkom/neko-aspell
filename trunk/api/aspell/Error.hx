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

class Error {
	var e: Void; 			// error abstract;
	var c: Void;			// config abstract;
	var speller: Speller;
	
	public function new(e: Void, c: Void) {
		this.e = e;
		this.c = c;
		speller = null;
	}
	
	public var number (getNumber,null): Int;
	static var _nas_error_number = neko.Lib.load("aspell","nas_error_number",1);
	function getNumber(): Int {
		return _nas_error_number(e);
	}
	
	public var message (getMessage,null): String;
	static var _nas_error_message = neko.Lib.load("aspell","nas_error_message",1);
	function getMessage(): String {
		return new String(_nas_error_message(e));
	}
	
	static var _nas_error_to_speller = neko.Lib.load("aspell","nas_error_to_speller",2);
	public function toSpeller(): Speller {
		if (speller==null)
			return speller = new Speller(_nas_error_to_speller(e,c));
		else
			return speller;
	}
}